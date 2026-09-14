#include "system.h"

System::System(Cartridge& cartridge)
{
    cpu = std::make_unique<CPU>();
    mem = std::make_unique<Memory>();
    ppu = std::make_unique<PPU>(cartridge);
    apu = std::make_unique<APU>();
    
    bus = std::make_unique<Bus>(*cpu, *mem, *ppu, *apu, cartridge);
}

void System::Reset()
{
    cpu->Reset();
    ppu->Reset();
    apu->Reset();
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
    const u32 cycles = cpu->Step();

    for (u32 i = 0; i < cycles * 3; ++i)
    {
        ppu->Step();
        if (ppu->IsNMIRequested())
        {
            cpu->RequestNMI();
            ppu->ClearNMIRequest();
        }
    }    

    return cycles;
}

u32 System::StepFrame()
{
    u32 cycles = 0;
    while (!ppu->IsFrameComplete())
    {
        cycles += Step();
    }

    ppu->ClearFrameComplete();
    return cycles;
}

bool System::IsFrameComplete() const
{
    return ppu->IsFrameComplete();
}

std::span<const Pixel> System::GetFramebuffer()
{
    return ppu->GetFramebuffer();
}
