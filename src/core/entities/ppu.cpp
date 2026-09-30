#include "bus.h"
#include "ppu.h"

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
    odd_frame_ = false;

    ctrl_ = 0;
    mask_ = 0;
    status_ = 0;

    v_ = 0;
    t_ = 0;
    x_ = 0;
    w_ = false;

    read_buffer_ = 0;

    pattern_shift_low_ = 0;
    pattern_shift_high_ = 0;
    attribute_shift_low_ = 0;
    attribute_shift_high_ = 0;

    // Sprites
    sprite_eval_count_ = 0;
    sprite_render_count_ = 0;
    sprite_eval_index_ = 0;
    sprite_eval_oam_address_ = 0;
    sprite_eval_overflow_ = false;

    for (Sprite& sprite : sprite_scanline_)
    {
        sprite = {};
    }
}

void PPU::Step()
{
    Debug({
        .event = DebugEventType::PPUTiming,
        .ppu_scanline = scanline_,
        .ppu_cycle = cycle_
    });

    const bool render_enabled =
        GetFlag(mask_, PPUMaskFlag::EnableBackground) ||
        GetFlag(mask_, PPUMaskFlag::EnableSprites);

    // Skip cycle 0 on odd frames
    if (scanline_ == 261 && cycle_ == 0 && render_enabled && odd_frame_)
    {
        cycle_ = 1;
        return;
    }

    // Clear vblank interrupt
    if (scanline_ == 261 && cycle_ == 1)
    {
        SetFlag(status_, PPUStatusFlag::VerticalBlank, false);
        SetFlag(status_, PPUStatusFlag::SpriteZeroHit, false);
        SetFlag(status_, PPUStatusFlag::SpriteOverflow, false);
    }

    bool render_scanlines = scanline_ <= 239 || scanline_ == 261;

    if (render_scanlines && render_enabled)
    {
        // Reset sprites
        if (cycle_ == 1)
        {
            sprite_eval_index_ = 0;
            sprite_eval_count_ = 0;
        }

        // Background rendering/fetching
        if (cycle_ >= 1 && cycle_ <= 256)
        {
            BackgroundRenderFetch();
        }

        if (cycle_ >= 1 && cycle_ <= 63)
        {
            ClearSecondaryOAM();
        }

        if (cycle_ >= 65 && cycle_ <= 256)
        {
            EvaluateSprites();
        }

        // Increment coarse and fine Y
        if (cycle_ == 256)
        {
            IncrementY();
        }

        if (cycle_ == 257)
        {
            // Copy horizontal scroll bits from temporary register
            HorizontalTransfer();

            // Save the number of sprite before evaluating the next scanline
            sprite_render_count_ = sprite_eval_count_;
        }

        // Sprite fetching
        if (cycle_ >= 257 && cycle_ <= 320)
        {
            FetchSpriteData();
        }

        // Copy vertical scroll bits from temporary register
        if (scanline_ == 261 && cycle_ >= 280 && cycle_ <= 304)
        {
            VerticalTransfer();
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

    if (scanline_ <= 239 && cycle_ >= 1 && cycle_ <= 256)
    {
        RenderPixel();
    }

    if (
        render_scanlines &&
        render_enabled &&
        ((cycle_ >= 1 && cycle_ <= 256) || (cycle_ >= 321 && cycle_ <= 336))
    )
    {
        ShiftBackgroundRegisters();
    }

    // trigger vblank interrupt
    if (scanline_ == 241 && cycle_ == 1)
    {
        Debug({
            .event = DebugEventType::VBlank
        });

        SetFlag(status_, PPUStatusFlag::VerticalBlank, true);
        if (GetFlag(ctrl_, PPUCtrlFlag::NMIEnable))
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
            Debug({
                .event = DebugEventType::Frame
            });

            scanline_ = 0;
            odd_frame_ = !odd_frame_;
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
        {
            // TODO: implement open bus behaviour
            return 0x00;
        }

        // PPU Status
        case 0x2002: {
            const u8 stat = status_;
            SetFlag(status_, PPUStatusFlag::VerticalBlank, false);
            w_ = false;
            return stat;
        }

        // TODO: return 0xff if reading during clear of secondary OAM
        case 0x2004: return oam_[oam_addr_];

        // VRAM data
        case 0x2007:
        {
            const u16 address = v_ & 0x3fff;
            u8 result = 0x00;

            // Palette reads are not delayed, and the buffer reads from the nametable mirror
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

            v_ += GetFlag(ctrl_, PPUCtrlFlag::IncrementMode) ? 32 : 1;
            v_ &= 0x7fff;

            return result;
        }

        default: NESFERATU_THROW("Unmapped address: 0x{:04x}", address);
    }
}

void PPU::CPUWrite(u16 address, u8 data)
{
    switch (address)
    {
        // PPU control
        case 0x2000:
        {
            const bool prev_nmi = GetFlag(ctrl_, PPUCtrlFlag::NMIEnable);

            ctrl_ = data;
            
            // Write nametable selection
            t_ = (t_ & 0xf3ff) | ((data & 0x03) << 10);

            // Trigger NMI request if NMI is enable while vblank is active
            const bool nmi_enable = GetFlag(ctrl_, PPUCtrlFlag::NMIEnable);
            if (!prev_nmi && nmi_enable && GetFlag(status_, PPUStatusFlag::VerticalBlank))
            {
                nmi_requested_ = true;
            }
        }; break;

        // PPU mask
        case 0x2001: 
        {
            mask_ = data;
        }; break;

        // PPU status, ignore write
        case 0x2002: break;

        // OAM Address
        case 0x2003:
        {
            oam_addr_ = data;
        }; break;

        // OAM Data
        case 0x2004:
        {
            // Ignore writing during rendering
            bool rendering_enabled =
                GetFlag(mask_, PPUMaskFlag::EnableBackground) ||
                GetFlag(mask_, PPUMaskFlag::EnableSprites);

            bool render_scanlines = scanline_ <= 239 || scanline_ == 261;

            if (rendering_enabled && render_scanlines)
            {
                return;
            }

            oam_[oam_addr_] = data;
            ++oam_addr_;
        }; break;

        // Scroll register
        case 0x2005:
        {
            if (!w_)
            {
                // First write: coarse X and fine X
                t_ = (t_ & 0x7fe0) | (data >> 3);
                x_ = data & 0x07;
            }
            else
            {
                // Second write: coarse Y and fine Y
                t_ = (t_ & 0x0c1f) | (((data & 0x07) << 12) | ((data & 0xf8) << 2));
            }

            w_ = !w_;
        }; break;

        // VRAM address
        case 0x2006:
        {
            if (!w_)
            {
                // First write: uppwer VRAM address
                t_ = (t_ & 0x00ff) | (static_cast<u16>(data & 0x3f) << 8);
            }
            else
            {
                // Second write: lower VRAM address
                t_ = (t_ & 0x7f00) | data;
                v_ = t_;
            }

            w_ = !w_;
        }; break;

        // VRAM data
        case 0x2007:
        {
            PPUWrite(v_ & 0x3fff, data);
            v_ += GetFlag(ctrl_, PPUCtrlFlag::IncrementMode) ? 32 : 1;
            v_ &= 0x7fff;
        }; break;

        default: NESFERATU_THROW("Unmapped address: 0x{:04x}", address);
    }
}

u8 PPU::PPURead(u16 address)
{
    switch (address)
    {
        // Cartridge CHR ROM/RAM
        case 0x0000 ... 0x1fff: return bus_->PPURead(address);

        // Nametable RAM
        case 0x2000 ... 0x3eff: return vram_[MapNametableAddress(address)];

        // Palette RAM
        case 0x3f00 ... 0x3fff: return palettes_[GetPaletteIndex(address)];
        
        default: NESFERATU_THROW("Unmapped PPU address: 0x{:04x}", address);
    }
}

void PPU::PPUWrite(u16 address, u8 val)
{
    switch (address)
    {
        // Cartridge CHR ROM/RAM
        // TODO: I should not be able to write into ROM
        case 0x0000 ... 0x1fff: bus_->PPUWrite(address, val); break;

        // Nametable RAM
        case 0x2000 ... 0x3eff: vram_[MapNametableAddress(address)] = val; break;

        // Palette RAM
        case 0x3f00 ... 0x3fff: palettes_[GetPaletteIndex(address)] = val & 0x3f; break;

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

    u8 physical_page;
    switch (cartridge_.GetMirroring())
    {
        case Mirroring::Vertical: physical_page = nametable & 1; break;
        case Mirroring::Horizontal: physical_page = nametable >> 1; break;
        case Mirroring::OneScreenLower: physical_page = 0; break;
        case Mirroring::OneScreenUpper: physical_page = 1; break;
    }

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
            LoadBackgroundShiftRegisters();

            // Fetch nametable byte
            next_tile_id_ = PPURead(0x2000 | (v_ & 0x0fff));
        }; break;

        case 3:
        {
            // Fetch attribute byte
            const u16 address = 0x23c0 | (v_ & 0x0c00) | ((v_ >> 4) & 0x0038) | ((v_ >> 2) & 0x0007);
            next_tile_attribute_ = PPURead(address);

            const u8 quadrant =
                ((v_ >> 4) & 0x04) |
                ((v_ >> 0) & 0x02);

            next_tile_attribute_ = (next_tile_attribute_ >> quadrant) & 0x03;
        }; break;

        case 5:
        {
            // Fetch pattern low
            const u16 address = (GetFlag(ctrl_, PPUCtrlFlag::BackgroundPattern) ? 0x1000 : 0x0000) +
                next_tile_id_ * 16 + ((v_ >> 12) & 0x0007);
            next_tile_pattern_low_ = PPURead(address);
        }; break;

        case 7:
        {
            // Fetch pattern high
            const u16 address = (GetFlag(ctrl_, PPUCtrlFlag::BackgroundPattern) ? 0x1000 : 0x0000) +
                next_tile_id_ * 16 + ((v_ >> 12) & 0x0007) + 8;
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
            coarse_y = 0;
        }
        else
        {
            ++coarse_y;
        }

        v_ = (v_ & ~0x03e0) | (coarse_y << 5);
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
    // Max 8 sprites per scanline
    if (sprite_eval_index_ >= 64) return;

    const u8 base = sprite_eval_index_ * 4;
    const u8 y = oam_[base];
    ++sprite_eval_index_;

    u32 next_scanline = scanline_ == 261 ? 0 : scanline_;
    bool on_scanline =
        next_scanline >= y &&
        next_scanline < static_cast<u32>(y + (GetFlag(ctrl_, PPUCtrlFlag::SpriteSize) ? 16u : 8u));

    // The sprite is not on this scanline, exit
    if (!on_scanline) return;

    if (sprite_eval_count_ < 8)
    {
        const u8 sec_index = sprite_eval_count_ * 4;
        secondary_oam_[sec_index + 0] = oam_[base + 0];
        secondary_oam_[sec_index + 1] = oam_[base + 1];
        secondary_oam_[sec_index + 2] = oam_[base + 2];
        secondary_oam_[sec_index + 3] = oam_[base + 3];

        ++sprite_eval_count_;
    }
    else
    {
        SetFlag(status_, PPUStatusFlag::SpriteOverflow, true);
    }
}

void PPU::DummyFetches()
{
    if (cycle_ == 337 || cycle_ == 339)
    {
        next_tile_id_ = PPURead(0x2000 | (v_ & 0x0fff));
    }
}

void PPU::LoadBackgroundShiftRegisters()
{
    pattern_shift_low_ = (pattern_shift_low_ & 0xff00) | next_tile_pattern_low_;
    pattern_shift_high_ = (pattern_shift_high_ & 0xff00) | next_tile_pattern_high_;

    const u8 attribute_low = next_tile_attribute_ & 0x01;
    const u8 attribute_high = (next_tile_attribute_ >> 1) & 0x01;

    attribute_shift_low_ = (attribute_shift_low_ & 0xff00) | ((attribute_low != 0) ? 0xff : 0x00);
    attribute_shift_high_ = (attribute_shift_high_ & 0xff00) | ((attribute_high != 0) ? 0xff : 0x00);
}

void PPU::ShiftBackgroundRegisters()
{
    pattern_shift_low_ <<= 1;
    pattern_shift_high_ <<= 1;
    attribute_shift_low_ <<= 1;
    attribute_shift_high_ <<= 1;
}

void PPU::ClearSecondaryOAM()
{
    if ((cycle_ % 2) == 0)
    {
        secondary_oam_[cycle_ / 2] = 0xff;
    }
}

void PPU::FetchSpriteData()
{
    const u8 sprite_index = ((cycle_ - 257) / 8);

    // No need to fetch more sprites than needed
    if (sprite_index >= sprite_render_count_) return;

    Sprite& sprite = sprite_scanline_[sprite_index];
    const u8 secondary = sprite_index * 4;

    sprite.y = secondary_oam_[secondary + 0];
    sprite.tile = secondary_oam_[secondary + 1];
    sprite.attributes = secondary_oam_[secondary + 2];
    sprite.x = secondary_oam_[secondary + 3];

    const u8 target_scanline = scanline_ == 261 ? 0 : scanline_;
    u8 row = target_scanline - sprite.y;

    // Vertical flip
    if (sprite.attributes & 0x80)
    {
        row = 7 - row;
    }

    // TODO: reimplement for 8x16 sprites (check flag)
    const u16 table = GetFlag(ctrl_, PPUCtrlFlag::SpritePattern) ? 0x1000 : 0x0000;
    const u16 pattern_address = table + sprite.tile * 16  + row;

    const u8 phase = (cycle_ - 257) % 8;
    switch (phase)
    {
        case 5:
        {
            sprite.pattern_low = PPURead(pattern_address);
        }; break;

        case 7:
        {
            sprite.pattern_high = PPURead(pattern_address + 8);
        }; break;
    }
}

u8 PPU::GetPaletteIndex(u16 address) const
{
    u8 index = (address - 0x3f00) & 0x1f;

    if ((index & 0x03) == 0 && index >= 0x10)
    {
        index -= 0x10;
    }

    return index;
}

u8 PPU::GetBackgroundColorIndex(u8 palette, u8 pixel) const
{
    return pixel == 0 ? palettes_[0] : palettes_[palette * 4 + pixel];
}

Pixel PPU::GetColor(u8 palette_index) const
{
    return NES_PALETTE[palette_index & 0x3f];
}

void PPU::RenderPixel()
{
    u8 palette = 0x00;
    u8 pattern = 0x00;
    
    if (GetFlag(mask_, PPUMaskFlag::EnableBackground))
    {
        const u16 bit = 0x8000 >> x_;

        // Generate background pixel
        const u8 pattern_low = (pattern_shift_low_ & bit) > 0;
        const u8 pattern_high = (pattern_shift_high_ & bit) > 0;
        pattern = (pattern_high << 1) | pattern_low;

        const u8 palette_low = (attribute_shift_low_ & bit) > 0;
        const u8 palette_high = (attribute_shift_high_ & bit) > 0;
        palette = (palette_high << 1) | palette_low;
    }

    // TODO: move the RenderPixel function inside the render check in the step function
    // And remove the flag checks from here

    u8 sprite_pattern = 0;
    u8 sprite_palette = 0;
    bool sprite_priority = false;
    bool sprite_present = false;
    bool sprite_zero = false;

    if (GetFlag(mask_, PPUMaskFlag::EnableSprites))
    {
        for (u8 i = 0; i < sprite_render_count_; ++i)
        {
            const Sprite sprite = sprite_scanline_[i];
            const u16 x = cycle_ - 1u;

            // Pixel is out of sprite bounds
            if (x < sprite.x || x >= (sprite.x + 8u))
            {
                continue;
            }

            u8 sprite_x = x - sprite.x;
            
            // Horizontal flip
            if (sprite.attributes & 0x40)
            {
                sprite_x = 7 - sprite_x;
            }

            const u8 bit = 7 - sprite_x;

            const u8 low = (sprite.pattern_low >> bit) & 1;
            const u8 high = (sprite.pattern_high >> bit) & 1;
            const u8 pixel = (high << 1) | low;

            // Pixel is transparent, move to the next one
            if (pixel == 0x00)
            {
                continue;
            }

            sprite_pattern = pixel;
            sprite_palette = sprite.attributes & 0x03;
            sprite_priority = (sprite.attributes & 0x20) != 0;
            sprite_present = true;
            sprite_zero = (i == 0);

            // Sprite found, no need to search further
            // Sprite with lower OAM address has priority on overlap
            break;
        }
    }

    const u16 screen_x = cycle_ - 1;
    const bool background_visible = pattern != 0 && (screen_x >= 8 || GetFlag(mask_, PPUMaskFlag::EnableBackgroundLeft));
    const bool sprite_visible = sprite_present && (screen_x >= 8 || GetFlag(mask_, PPUMaskFlag::EnableSpritesLeft));

    // TODO: refactor this awful thing
    u8 final_palette;
    if (!background_visible && !sprite_visible)
    {
        final_palette = palettes_[0];
    }
    else if (!background_visible)
    {
        final_palette = palettes_[0x10 + sprite_palette * 4 + sprite_pattern];
    }
    else if (!sprite_visible)
    {
        final_palette = GetBackgroundColorIndex(palette, pattern);
    }
    else if (!sprite_priority)
    {
        final_palette = palettes_[0x10 + sprite_palette * 4 + sprite_pattern];
    }
    else
    {
        final_palette = GetBackgroundColorIndex(palette, pattern);
    }

    const std::size_t framebuffer_index =
        static_cast<std::size_t>(scanline_) * FRAMEBUFFER_WIDTH +
        static_cast<std::size_t>(cycle_) - 1;

    // TODO: implement emphasis and grayscale flags
    framebuffer_[framebuffer_index] = GetColor(final_palette);

    // Sprite zero hit flag
    // TODO: implement correct leftmost masking
    if (!GetFlag(status_, PPUStatusFlag::SpriteZeroHit) &&
        sprite_zero &&
        background_visible &&
        sprite_visible &&
        scanline_ < 239 &&
        screen_x != 255)
    {
        SetFlag(status_, PPUStatusFlag::SpriteZeroHit, true);
    }
}

std::array<Pixel, 32> PPU::DumpPalette() const
{
    std::array<Pixel, 32> palette;

    for (u8 i = 0; i < 32; ++i)
    {
        palette[i] = GetColor(palettes_[GetPaletteIndex(0x3f00 + i)]);
    }

    return palette;
}
