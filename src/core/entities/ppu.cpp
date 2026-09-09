#include "ppu.h"

#include <stdexcept>
#include <format>

#include "utils/exception.h"
#include "bus.h"

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
    v_ = 0;
    t_ = 0;
    x_ = 0;
    w_ = false;

    // Reset background pipeline
    nametable_byte_ = 0;
    attribute_byte_ = 0;
    pattern_low_ = 0;
    pattern_high_ = 0;

    pattern_shift_low_ = 0;
    pattern_shift_high_ = 0;

    attribute_shift_low_ = 0;
    attribute_shift_high_ = 0;
}

void PPU::Step()
{
    frame_complete_ = false;

    if (scanline_ == 261)
    {
        if (GetFlag(mask, PPUMaskFlag::EnableBackground) || GetFlag(mask, PPUMaskFlag::EnableSprites))
        {
            // Background rendering/fetching
            if (cycle_ >= 1 && cycle_ <= 256)
            {
                BackgroundRenderFetch();
            }

            // Background rendering/fetching
            if (cycle_ >= 321 && cycle_ <= 336)
            {
                BackgroundRenderFetch();
            }

            // Increment coarse and fine Y
            if (cycle_ == 256)
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

            // Copy horizontal scroll bits from temporary register
            if (cycle_ == 257)
            {
                v_ = (v_ & 0xfbe0) | (t_ & 0x041f);
            }

            // Sprite evaluation/fetching
            if (cycle_ >= 257 && cycle_ <= 320)
            {
                // TODO: fill
            }

            // Copy vertical scroll bits from temporary register
            if (cycle_ >= 280 && cycle_ <= 304)
            {
                v_ = (v_ & 0x041f) | (t_ & 0x7be0);
            }

            // Nabletable fetches
            if (cycle_ >= 337 && cycle_ <= 340)
            {
                const u16 address = (0x2000 | (v_ & 0x0fff));
                next_tile_id_ = PPURead(address);
            }
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
        case 0x0000: // PPUCTRL
        case 0x0001: // PPUMASK
        case 0x0003: // OAMADDR
        case 0x0005: // PPUSCROLL
        case 0x0006: // PPUADDR
        case 0x2014: // OAM DMA
            throw std::runtime_error(std::format("[PPU] Address is write-only: 0x{:04x}", address));

        case 0x0002: return status; // TODO: implement open-bus behavior (random on lower bits)
        case 0x0004: return oam_data;
        case 0x0007: return data;

        default: throw std::runtime_error(std::format("[PPU] Unmapped address: 0x{:04x}", address));
    }
}

void PPU::CPUWrite(u16 address, u8 val)
{
    throw std::logic_error(std::format("[PPU] Writing not implemented. Address 0x{:04x}", address));
}

Pixel PPU::RenderPixel(u32 x, u32 y)
{
    constexpr u16 NAMETABLE_BASE = 0x2000;
    constexpr u16 PATTERN_TABLE_BASE = 0x0000;

    constexpr u32 TILE_SIZE = 8;
    constexpr u32 NAMETABLE_WIDTH = 32;

    const u32 tile_x = x / TILE_SIZE;
    const u32 tile_y = y / TILE_SIZE;

    const u32 pixel_x = x % TILE_SIZE;
    const u32 pixel_y = y % TILE_SIZE;

    const u16 nametable_address = NAMETABLE_BASE + tile_y * NAMETABLE_WIDTH + tile_x;
    const u8 tile_index = PPURead(nametable_address);
    const u16 pattern_address = PATTERN_TABLE_BASE + static_cast<u16>(tile_index) * 16;

    const u8 plane0 = PPURead(pattern_address + pixel_y);
    const u8 plane1 = PPURead(pattern_address + pixel_y + 8);

    const u8 low = (plane0 >> (7 - pixel_x)) & 1;
    const u8 high = (plane1 >> (7 - pixel_x)) & 1;
    const u8 color_index = low | (high << 1);

    const u16 attr_address = 0x23c0 + (tile_y / 4) * 8 + (tile_x / 4);
    const u8 attribute = PPURead(attr_address);

    const u8 quadrant_x = (tile_x % 4) / 2;
    const u8 quadrant_y = (tile_y % 4) / 2;
    const u8 shift = (quadrant_y * 4 + quadrant_x * 2);
    const u8 palette = (attribute >> shift) & 0x03;

    const u16 palette_address = color_index == 0 ? 0x3f00 : 0x3f00 + palette * 4 + color_index;
    const u8 nes_color = PPURead(palette_address);

    return NESColor(nes_color);
}

u8 PPU::PPURead(u16 address)
{
    switch (address)
    {
        // Cartridge CHR ROM/RAM
        case 0x0000 ... 0x1fff: return bus_->PPURead(address);

        // Internal VRAM + mirror
        case 0x2000 ... 0x3eff: return vram_[MapNametableAddress(address)];

        // Palette RAM
        // TODO: implement correct mirrors
        case 0x3f00 ... 0x3fff: return palettes_[address - 0x3f00];
        
        default: NESFERATU_THROW("Unmapped PPU address: 0x{:04x}", address);
    }
}

void PPU::PPUWrite(u16 address, u8 val)
{
    NESFERATU_THROW("Not implemented.");
}

bool PPU::IsFrameComplete()
{
    return frame_complete_;
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

constexpr void PPU::SetFlag(u8 reg, u8 flag, bool val)
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

u16 PPU::GetBackgroundPatternTableAddress() const
{
    return GetFlag(ctrl, PPUCtrlFlag::BackgroundPattern) ? 0x0000 : 0x1000;
}
