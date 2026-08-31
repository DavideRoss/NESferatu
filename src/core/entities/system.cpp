#include "system.h"

System::System(Cartridge& cartridge)
{
    cpu = std::make_unique<CPU>();
    mem = std::make_unique<Memory>();
    bus = std::make_unique<Bus>(*cpu, *mem, cartridge);
}

void System::Reset()
{
    cpu->Reset();
}

void System::Execute(const u32 cycles)
{
    u32 curr_cycles = 0;
    while (curr_cycles < cycles)
    {
        curr_cycles += cpu->Step();
    }
}

u32 System::Step()
{
    return cpu->Step();
}

std::span<const Pixel> System::GetFramebuffer()
{
    // TODO: get framebuffer from PPU

    Pixel pixels[] = {
        { 0xff, 0x00, 0x00 },
        { 0x00, 0xff, 0x00 },
        { 0x00, 0x00, 0xff },
        { 0xff, 0xff, 0x00 },
        { 0xff, 0x00, 0xff },
        { 0x00, 0xff, 0xff }
    };

    for (u32 i = 0; i < 256 * 240; i++)
    {
        framebuffer_[i] = pixels[i % 6];
    }

    return framebuffer_;
}
