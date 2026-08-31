#pragma once

#include "cpu.h"
#include "memory.h"
#include "bus.h"

struct Pixel
{
    u8 r;
    u8 g;
    u8 b;
};

class System
{

public:
    System(Cartridge& cartridge);

    void Reset();
    void Execute(const u32 cycles);
    u32 Step();

    // TODO: reintroduce const function
    std::span<const Pixel> GetFramebuffer();

public:
    std::unique_ptr<Bus> bus;
    std::unique_ptr<CPU> cpu;
    std::unique_ptr<Memory> mem;

private:
    std::array<Pixel, 256 * 240> framebuffer_;
};
