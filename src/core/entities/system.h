#pragma once

#include "bus.h"
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

    // TODO: remove when proper timing is implemeted
    void StepFrame();

    std::span<const Pixel> GetFramebuffer();

public:
    std::unique_ptr<Bus> bus;
    std::unique_ptr<CPU> cpu;
    std::unique_ptr<Memory> mem;
    std::unique_ptr<PPU> ppu;

private:
    std::array<Pixel, 256 * 240> framebuffer_;
};
