#include "system.h"

System::System(Cartridge& cartridge)
{
    cpu = std::make_unique<CPU>();
    mem = std::make_unique<Memory>();
    ppu = std::make_unique<PPU>(cartridge);
    
    bus = std::make_unique<Bus>(*cpu, *mem, *ppu, cartridge);
}

void System::Reset()
{
    cpu->Reset();
    ppu->Reset();
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

void System::StepFrame()
{
    while (!ppu->IsFrameComplete())
    {
        // TODO: enable CPU again when all instructions are implemented
        // const u32 cpu_cycles = cpu->Step();
        const u32 cpu_cycles = 10;
        for (u32 i = 0; i < cpu_cycles; i++)
        {
            ppu->Step();
        }
    }
}

std::span<const Pixel> System::GetFramebuffer()
{
    return ppu->GetFramebuffer();
}
