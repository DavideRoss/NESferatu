#include "system.h"

System::System()
{
    cpu = new CPU();
    mem = new Memory();
    bus = new Bus(cpu, mem);
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
