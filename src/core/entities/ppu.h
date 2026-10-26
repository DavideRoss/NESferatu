#pragma once

#include <span>

#include "data/palette.h"
#include "entities/cartridge.h"
#include "debugger/debuggable.h"

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

enum PPUStatusFlag : u8
{
    SpriteOverflow = (1 << 5),
    SpriteZeroHit = (1 << 6),
    VerticalBlank = (1 << 7)
};

enum DebugViewMode : u8
{
    None,
    Palette,
    FineX,
    PixelPresence,
    Nametable
};

struct Sprite
{
    u8 y;
    u8 tile;
    u8 attributes;
    u8 x;

    u8 pattern_low;
    u8 pattern_high;
};

class PPU : public Debuggable
{
public:
    PPU();

    void ConnectBus(Bus* bus);
    void Reset();

    void Step();

    u8 CPURead(u16 address);
    void CPUWrite(u16 address, u8 data);

    bool IsNMIRequested() const;
    bool IsFrameComplete() const;

    void ClearNMIRequest();
    void ClearFrameComplete();

    std::span<const Pixel> GetFramebuffer();

    void Serialize(class Serializer& serializer);
    void Deserialize(class Deserializer& deserializer);

    // Debug functions
    std::array<Pixel, 32> DumpPalette() const;
    void SetDebugViewMode(DebugViewMode mode);

private:
    u8 PPURead(u16 address);
    void PPUWrite(u16 address, u8 val);

    constexpr u8 GetFlag(u8 reg, u8 flag) const;
    constexpr void SetFlag(u8& reg, u8 flag, bool val);
    
    // TODO: sort functions, even in the cpp file
    void BackgroundRenderFetch();
    void IncrementY();
    void HorizontalTransfer();
    void VerticalTransfer();
    void EvaluateSprites();
    void DummyFetches();
    void LoadBackgroundShiftRegisters();
    void ShiftBackgroundRegisters();
    void ClearSecondaryOAM();
    void FetchSpriteData();
    
    u16 MapNametableAddress(u16 address);
    u8 GetPaletteIndex(u16 address) const;
    u8 GetBackgroundColorIndex(u8 palette, u8 pixel) const;

    Pixel GetColor(u8 palette_index) const;
    void RenderPixel();

private:
    static constexpr u32 FRAMEBUFFER_WIDTH = 256;
    static constexpr u32 FRAMEBUFFER_HEIGHT = 240;

    static constexpr u32 CYCLES_PER_SCANLINE = 341;
    static constexpr u32 SCANLINES_PER_FRAME = 262;

private:
    Bus* bus_ = nullptr;

    u32 cycle_ = 0;
    u32 scanline_ = 0;
    bool frame_complete_ = false;
    bool nmi_requested_ = false;
    bool odd_frame_ = false;

    // Registers
    u8 ctrl_;
    u8 mask_;
    u8 status_;
    u8 oam_addr_;

    // PPU registers    During rendering        Outside rendering
    u16 v_;          // Scroll position         Current VRAM address
    u16 t_;          // Coarse X + start y      Temporary VRAM address
    u8 x_;           // Fine X during scroll
    bool w_;         // Write latch
    
    u8 next_tile_id_ = 0;
    u8 next_tile_attribute_ = 0;
    u8 next_tile_pattern_low_ = 0;
    u8 next_tile_pattern_high_ = 0;

    u16 pattern_shift_low_ = 0;
    u16 pattern_shift_high_ = 0;
    u16 attribute_shift_low_ = 0;
    u16 attribute_shift_high_ = 0;

    u8 read_buffer_ = 0x00;

    // Sprites
    std::array<Sprite, 8> sprite_scanline_;
    u8 sprite_render_count_;
    u8 sprite_eval_count_;
    u8 sprite_eval_index_ = 0;
    u8 sprite_eval_oam_address_ = 0;
    bool sprite_eval_overflow_ = false;

    // Framebuffer and memories
    std::array<Pixel, FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT> framebuffer_;

    std::array<u8, 0x800> vram_;
    std::array<u8, 0x20> palettes_;
    std::array<u8, 256> oam_;
    std::array<u8, 32> secondary_oam_;

    // Debug variable
    DebugViewMode debug_view_mode_ = DebugViewMode::None;
};
