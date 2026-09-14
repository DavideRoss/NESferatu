#include "ppu.h"
#include "bus.h"

#include "utils/exception.h"

PPU::PPU(Cartridge &cartridge)
    : cartridge_(cartridge)
{
}

void PPU::ConnectBus(Bus *bus)
{
    bus_ = bus;
}

void PPU::Reset()
{
    scanline_ = 0;
    cycle_ = 0;

    frame_complete_ = false;
    nmi_requested_ = false;

    ctrl = 0;
    mask = 0;
    status = 0;

    v_ = 0;
    t_ = 0;
    fine_x_ = 0;
    w_ = false;

    read_buffer_ = 0;

    pattern_shift_low_ = 0;
    pattern_shift_high_ = 0;
    attribute_shift_low_ = 0;
    attribute_shift_high_ = 0;
}

void PPU::Step()
{
    frame_complete_ = false;

    const bool render_enabled =
        GetFlag(mask, PPUMaskFlag::EnableBackground) ||
        GetFlag(mask, PPUMaskFlag::EnableSprites);

    // Pre-render scanline
    if (scanline_ == 261)
    {
        // Clear vblank interrupt
        if (cycle_ == 1)
        {
            status &= ~(PPUStatusFlag::VerticalBlank | PPUStatusFlag::SpriteZeroHit | PPUStatusFlag::SpriteOverflow);
        }

        if (render_enabled)
        {
            // Background rendering/fetching
            if (cycle_ >= 1 && cycle_ <= 256)
            {
                BackgroundRenderFetch();
            }

            // Increment coarse and fine Y
            if (cycle_ == 256)
            {
                IncrementY();
            }

            // Copy horizontal scroll bits from temporary register
            if (cycle_ == 257)
            {
                HorizontalTransfer();
            }

            // Sprite evaluation/fetching
            if (cycle_ >= 257 && cycle_ <= 320)
            {
                EvaluateSprites();
            }

            // Copy vertical scroll bits from temporary register
            if (cycle_ >= 280 && cycle_ <= 304)
            {
                VerticalTransfer();
            }

            // Nabletable dummy fetches
            if (cycle_ >= 337 && cycle_ <= 340)
            {
                DummyFetches();
            }
        }
    }

    // Visible scanlines
    if (scanline_ <= 239)
    {
        if (render_enabled)
        {
            if (cycle_ >= 1 && cycle_ <= 256)
            {
                BackgroundRenderFetch();
            }

            // Increment coarse and fine Y
            if (cycle_ == 256)
            {
                IncrementY();
            }

            // Copy horizontal scroll bits from temporary register
            if (cycle_ == 257)
            {
                HorizontalTransfer();
            }

            // Sprite evaluation/fetching
            if (cycle_ >= 257 && cycle_ <= 320)
            {
                EvaluateSprites();
            }

            if (cycle_ >= 321 && cycle_ <= 336)
            {
                BackgroundRenderFetch();
            }

            // Nabletable dummy fetches
            if (cycle_ >= 337 && cycle_ <= 340)
            {
                DummyFetches();
            }
        }

        if (GetFlag(mask, PPUMaskFlag::EnableBackground) && cycle_ >= 1 && cycle_ <= 256)
        {
            RenderPixel();
        }
    }

    // trigger vblank interrupt
    if (scanline_ == 241 && cycle_ == 1)
    {
        SetFlag(status, PPUStatusFlag::VerticalBlank, true);
        if (GetFlag(ctrl, PPUCtrlFlag::NMIEnable))
        {
            nmi_requested_ = true;
        }
    }
    
    // Cycle increment
    ++cycle_;
    if (cycle_ >= CYCLES_PER_SCANLINE)
    {
        cycle_ = 0;
        ++scanline_;

        if (scanline_ >= SCANLINES_PER_FRAME)
        {
            scanline_ = 0;
            frame_complete_ = true;
        }
    }
}

std::span<const Pixel> PPU::GetFramebuffer()
{
    return framebuffer_;
}

u8 PPU::CPURead(u16 address)
{
    switch (address)
    {
        case 0x2000: // PPUCTRL
        case 0x2001: // PPUMASK
        case 0x2003: // OAMADDR
        case 0x2005: // PPUSCROLL
        case 0x2006: // PPUADDR
        case 0x2014: // OAM DMA
            NESFERATU_THROW("Address is write-only: 0x{:04x}", address);

        case 0x2002: {
            const u8 stat = status;
            SetFlag(status, PPUStatusFlag::VerticalBlank, false);
            w_ = false;
            return stat;
        }

        case 0x2004: return oam_[oam_addr];

        // VRAM data
        case 0x2007:
        {
            const u16 address = v_ & 0x3fff;
            u8 result = 0x00;

            if (address >= 0x3f00)
            {
                result = PPURead(address);
                read_buffer_ = PPURead(address & 0x2fff);
            }
            else
            {
                result = read_buffer_;
                read_buffer_ = PPURead(address);
            }

            v_ += GetFlag(ctrl, PPUCtrlFlag::IncrementMode) ? 32 : 1;
            v_ &= 0x7fff;

            return result;
        }

        default: NESFERATU_THROW("Unmapped address: 0x{:04x}", address);
    }
}

void PPU::CPUWrite(u16 address, u8 val)
{
    switch (address)
    {
        // PPU control
        case 0x2000:
        {
            const bool prev_nmi = GetFlag(ctrl, PPUCtrlFlag::NMIEnable);

            ctrl = val;

            t_ = (t_ & 0xf3ff) | ((val & 0x03) << 10);

            // Trigger NMI request if NMI is enable while vblank is active
            const bool nmi_enable = GetFlag(ctrl, PPUCtrlFlag::NMIEnable);
            if (!prev_nmi && nmi_enable && GetFlag(status, PPUStatusFlag::VerticalBlank))
            {
                nmi_requested_ = true;
            }
        }; break;

        // PPU mask
        case 0x2001: 
        {
            mask = val;
        }; break;

        // OAM Address
        case 0x2003:
        {
            oam_addr = val;
        }; break;

        // OAM Data
        case 0x2004:
        {
            oam_[oam_addr] = val;
            ++oam_addr;
        }; break;

        // Scroll register
        case 0x2005:
        {
            if (!w_)
            {
                // First write
                t_ = (t_ & 0x7fe0) | (val >> 3);
                fine_x_ = val & 0x07;
            }
            else
            {
                // Second write
                t_ = (t_ & 0x0c1f) | (((val & 0x07) << 12) | ((val & 0xf8) << 2));
            }

            w_ = !w_;
        }; break;

        // VRAM address
        case 0x2006:
        {
            if (w_)
            {
                t_ = (t_ & 0x7f00) | val;
                v_ = t_;
            }
            else
            {
                t_ = (t_ & 0x00ff) | (static_cast<u16>(val & 0x3f) << 8);
            }

            w_ = !w_;
        }; break;

        // VRAM data
        case 0x2007:
        {
            PPUWrite(v_, val);
            v_ += GetFlag(ctrl, PPUCtrlFlag::IncrementMode) ? 32 : 1;
        }; break;

        default: NESFERATU_THROW("Unmapped address: 0x{:04x}", address);
    }
}

u8 PPU::PPURead(u16 address)
{
    address &= 0x3fff;
    switch (address)
    {
        // Cartridge CHR ROM/RAM
        case 0x0000 ... 0x1fff: return bus_->PPURead(address);

        // Nametable RAM
        case 0x2000 ... 0x3eff: return vram_[MapNametableAddress(address)];

        // Palette RAM
        // TODO: implement correct mirrors
        case 0x3f00 ... 0x3fff: return palettes_[address - 0x3f00];
        
        default: NESFERATU_THROW("Unmapped PPU address: 0x{:04x}", address);
    }
}

void PPU::PPUWrite(u16 address, u8 val)
{
    address &= 0x3fff;
    switch (address)
    {
        // Cartridge CHR ROM/RAM
        // TODO: I should not be able to write into ROM
        case 0x0000 ... 0x1fff: bus_->PPUWrite(address, val); break;

        // Nametable RAM
        case 0x2000 ... 0x3eff: vram_[MapNametableAddress(address)] = val; break;

        // Palette RAM
        case 0x3f00 ... 0x3fff: palettes_[address - 0x3f00] = val; break;

        default: NESFERATU_THROW("Unmapped PPU address: 0x{:04x}", address);
    }
}

bool PPU::IsNMIRequested() const
{
    return nmi_requested_;
}

bool PPU::IsFrameComplete() const
{
    return frame_complete_;
}

void PPU::ClearNMIRequest()
{
    nmi_requested_ = false;
}

void PPU::ClearFrameComplete()
{
    frame_complete_ = false;
}

u16 PPU::MapNametableAddress(u16 address)
{
    address = 0x2000 + ((address - 0x2000) & 0x0fff);

    const u16 offset = address - 0x2000;
    const u8 nametable = static_cast<u8>(offset / 0x0400);
    const u16 tile_offset = offset % 0x0400;

    u8 physical_page =
        cartridge_.metadata.vertical_mirroring ?
        nametable % 2 :
        nametable / 2;

    return static_cast<u16>(physical_page) * 0x0400 + tile_offset;
}

constexpr u8 PPU::GetFlag(u8 reg, u8 flag) const
{
    return ((reg & flag) > 0) ? 0x01 : 0x00;
}

constexpr void PPU::SetFlag(u8& reg, u8 flag, bool val)
{
    reg = val ? reg | flag : reg & ~flag;
}

void PPU::BackgroundRenderFetch()
{
    const u8 phase = cycle_ % 8;
    switch (phase)
    {
        case 0:
        {
            // Increment coarse X
            if ((v_ & 0x001f) == 31)
            {
                v_ &= ~0x001f;
                v_ ^= 0x0400;
            }
            else
            {
                ++v_;
            }
        }; break;

        case 1:
        {
            // Load shift registers
            pattern_shift_low_ = (pattern_shift_low_ & 0x00ff) | (next_tile_pattern_low_ << 8);
            pattern_shift_high_ = (pattern_shift_high_ & 0x00ff) | (next_tile_pattern_high_ << 8);

            const u8 attribute_low = next_tile_attribute_ & 0x01;
            const u8 attribute_high = (next_tile_attribute_ >> 1) & 0x01;

            attribute_shift_low_ = (attribute_shift_low_ & 0x00ff) | (attribute_low != 0 ? 0xff00 : 0x0000);
            attribute_shift_high_ = (attribute_shift_high_ & 0x00ff) | (attribute_high != 0 ? 0xff00 : 0x0000);

            // Fetch nametable byte
            next_tile_id_ = PPURead(0x2000 | (v_ & 0x0fff));
        }; break;

        case 3:
        {
            // Fetch attribute byte
            // NOTE: wtf is going on here?
            const u16 address = 0x23c0 | (v_ & 0x0c00) | ((v_ >> 4) & 0x0038) | ((v_ >> 2) & 0x0007);
            next_tile_attribute_ = PPURead(address);

            const u8 quadrant = ((v_ >> 4) & 0x04) | ((v_ >> 2) & 0x02);
            next_tile_attribute_ = (next_tile_attribute_ >> quadrant) & 0x03;
        }; break;

        case 5:
        {
            // Fetch pattern low
            const u16 address = GetBackgroundPatternTableAddress() + next_tile_id_ * 16 + ((v_ >> 12) & 0x0007);
            next_tile_pattern_low_ = PPURead(address);
        }; break;

        case 7:
        {
            // Fetch pattern high
            const u16 address = GetBackgroundPatternTableAddress() + next_tile_id_ * 16 + ((v_ >> 12) & 0x0007) + 8;
            next_tile_pattern_high_ = PPURead(address);
        }; break;
    }
}

void PPU::IncrementY()
{
    if ((v_ & 0x7000) != 0x7000)
    {
        v_ += 0x1000;
    }
    else
    {
        v_ &= ~0x7000;

        u16 coarse_y = (v_ & 0x03e0) >> 5;
        if (coarse_y == 29)
        {
            coarse_y = 0;
            v_ ^= 0x0800;
        }
        else if (coarse_y == 31)
        {
            v_ &= ~0x03e0;
        }
        else
        {
            v_ += 0x0020;
        }
    }
}

void PPU::HorizontalTransfer()
{
    v_ = (v_ & 0xfbe0) | (t_ & 0x041f);
}

void PPU::VerticalTransfer()
{
    v_ = (v_ & 0x041f) | (t_ & 0x7be0);
}

void PPU::EvaluateSprites()
{
}

void PPU::DummyFetches()
{
    const u16 address = (0x2000 | (v_ & 0x0fff));
    next_tile_id_ = PPURead(address);
}

void PPU::RenderPixel()
{
    // Generate background pixel
    // TODO: move them somewhere else
    constexpr u16 FRAMEBUFFER_WIDTH  = 256;
    constexpr u16 FRAMEBUFFER_HEIGHT = 240;

    const u8 pattern_low = (pattern_shift_low_ >> (15 - fine_x_)) & 0x01;
    const u8 pattern_high = (pattern_shift_high_ >> (15 - fine_x_)) & 0x01;
    const u8 pattern = (pattern_high << 1) | pattern_low;

    const u8 palette_low = (attribute_shift_low_ >> (15 - fine_x_)) & 0x01;
    const u8 palette_high = (attribute_shift_high_ >> (15 - fine_x_)) & 0x01;
    const u8 palette = (palette_high << 1) | palette_low;

    const u8 palette_index = pattern == 0 ? palettes_[0] : palettes_[palette * 4 + pattern];

    const std::size_t framebuffer_index =
        static_cast<std::size_t>(scanline_) * FRAMEBUFFER_WIDTH +
        static_cast<std::size_t>(cycle_) - 1;

    framebuffer_[framebuffer_index] = NESColor(palette_index);

    pattern_shift_low_ <<= 1;
    pattern_shift_high_ <<= 1;
    attribute_shift_low_ <<= 1;
    attribute_shift_high_ <<= 1;
}

u16 PPU::GetBackgroundPatternTableAddress() const
{
    return GetFlag(ctrl, PPUCtrlFlag::BackgroundPattern) ? 0x1000 : 0x0000;
}
