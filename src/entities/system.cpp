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
