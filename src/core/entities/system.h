#pragma once

#include "apu.h"
#include "bus.h"
#include "controllers.h"
#include "cpu.h"
#include "memory.h"
#include "ppu.h"

class System
{

public:
    System(Cartridge& cartridge);

    void Reset();
    void Execute(const u32 cycles);

    u32 Step();
    u32 StepFrame();

    bool IsFrameComplete() const;
    std::span<const Pixel> GetFramebuffer();

public:
    std::unique_ptr<Bus> bus;
    std::unique_ptr<CPU> cpu;
    std::unique_ptr<Memory> mem;
    std::unique_ptr<PPU> ppu;
    std::unique_ptr<APU> apu;
    std::unique_ptr<Controllers> controllers;
};
