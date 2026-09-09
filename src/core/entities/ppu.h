#pragma once

#include <span>

#include "entities/cartridge.h"

#include "utils/types.h"

class Bus;

enum PPUCtrlFlag : u8
{
    Nametable1 = (1 << 0),
    Nametable2 = (1 << 1),
    IncrementMode = (1 << 2),
    SpritePattern = (1 << 3),
    BackgroundPattern = (1 << 4),
    SpriteSize = (1 << 5),
    MasterSlave = (1 << 6),
    NMIEnable = (1 << 7)
};

// TODO: byte 5 and 6 (red and green) are reversed for PAL
enum PPUMaskFlag : u8
{
    Grayscale = (1 << 0),
    EnableBackgroundLeft = (1 << 1),
    EnableSpritesLeft = (1 << 2),
    EnableBackground = (1 << 3),
    EnableSprites = (1 << 4),
    EmphasizeRed = (1 << 5),
    EmphasizeGreen = (1 << 6),
    EmphasizeBlue = (1 << 7)
};

// TODO: implement open-bus behaviour
enum PPUStatusFlag : u8
{
    SpriteOverflow = (1 << 5),
    SpriteZeroHit = (1 << 6),
    VerticalBlank = (1 << 7)
};

struct Pixel
{
    u8 r;
    u8 g;
    u8 b;
};

class PPU
{
public:
    PPU(Cartridge& cartridge);

    void ConnectBus(Bus* bus);
    void Reset();

    void Step();

    u8 CPURead(u16 address);
    void CPUWrite(u16 address, u8 val);

    bool IsFrameComplete();
    std::span<const Pixel> GetFramebuffer();

private:
    // Pixel RenderPixel(u32 x, u32 y);

    u8 PPURead(u16 address);
    void PPUWrite(u16 address, u8 val);

    u16 MapNametableAddress(u16 address);

public:
    u8 ctrl;
    u8 mask;
    u8 status;
    u8 oam_addr;
    u8 oam_data;
    u8 addr;
    u8 data;
    u8 oam_dma;

private:
    constexpr u8 GetFlag(u8 reg, u8 flag) const;
    constexpr void SetFlag(u8 reg, u8 flag, bool val);

    // NOTE: cleanup functions
    void BackgroundRenderFetch();

    u16 GetBackgroundPatternTableAddress() const;

    // TODO: remove debug palette function and implement proper palette parsing from file
    constexpr Pixel NESColor(u8 color)
    {
        constexpr std::array<Pixel, 64> palette = {{
            {124, 124, 124}, {0, 0, 252},   {0, 0, 188},   {68, 40, 188},
            {148, 0, 132},   {168, 0, 32},  {168, 16, 0},  {136, 20, 0},
            {80, 48, 0},     {0, 120, 0},   {0, 104, 0},   {0, 88, 0},
            {0, 64, 88},     {0, 0, 0},     {0, 0, 0},     {0, 0, 0},

            {188, 188, 188}, {0, 120, 248}, {0, 88, 248},  {104, 68, 252},
            {216, 0, 204},   {228, 0, 88},  {248, 56, 0},  {228, 92, 16},
            {172, 124, 0},   {0, 184, 0},   {0, 168, 0},   {0, 168, 68},
            {0, 136, 136},   {0, 0, 0},     {0, 0, 0},     {0, 0, 0},

            {248, 248, 248}, {60, 188, 252}, {104, 136, 252}, {152, 120, 248},
            {248, 120, 248}, {248, 120, 120}, {248, 88, 48},  {248, 156, 56},
            {248, 184, 0},   {184, 248, 24},  {88, 216, 84},  {88, 248, 152},
            {0, 232, 216},   {120, 120, 120}, {0, 0, 0},     {0, 0, 0},

            {248, 248, 248}, {168, 228, 252}, {184, 184, 248}, {216, 184, 248},
            {248, 184, 248}, {248, 188, 188}, {248, 176, 156}, {248, 216, 168},
            {248, 228, 148}, {216, 248, 120}, {184, 248, 184}, {184, 248, 216},
            {0, 252, 252},   {216, 216, 216}, {0, 0, 0},     {0, 0, 0}
        }};

        return palette[color & 0x3Fu];
    }


private:
    static constexpr u32 SCREEN_WIDTH = 256;
    static constexpr u32 SCREEN_HEIGHT = 240;

    static constexpr u32 CYCLES_PER_SCANLINE = 341;
    static constexpr u32 SCANLINES_PER_FRAME = 262;

private:
    Bus* bus_ = nullptr;
    Cartridge& cartridge_;

    u32 cycle_ = 0;
    u32 scanline_ = 0;
    bool frame_complete_ = false;

    // PPU registers    During rendering        Outside rendering
    u16 v_;          // Scroll position         Current VRAM address
    u16 t_;          // Coarse X + start y      Temporary VRAM address
    u8 x_;           // Fine X during scroll
    bool w_;         // Write latch

    // Background pipeline
    // TODO: surely some of those are unused, discover which ones
    u8 nametable_byte_;
    u8 attribute_byte_;
    u8 pattern_low_;
    u8 pattern_high_;
    
    // TODO: lot of stuff here, simplify later. Move to a struct?
    u8 next_tile_id_ = 0;
    u8 next_tile_attribute_ = 0;
    u8 next_tile_pattern_low_ = 0;
    u8 next_tile_pattern_high_ = 0;

    // NOTE: NEEDED:
    u16 pattern_shift_low_ = 0;
    u16 pattern_shift_high_ = 0;
    u16 attribute_shift_low_ = 0;
    u16 attribute_shift_high_ = 0;

    u8 fine_x_ = 0;

    // Framebuffer and memories
    std::array<Pixel, SCREEN_WIDTH * SCREEN_HEIGHT> framebuffer_;

    std::array<u8, 0x800> vram_;
    std::array<u8, 0x20> palettes_;
};
