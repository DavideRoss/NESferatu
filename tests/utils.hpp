#pragma once

#include "entities/system.h"

namespace TestUtils
{

System* SetupSystem()
{
    CPU* cpu = new CPU();
    Memory* mem = new Memory();
    Bus* bus = new Bus(cpu, mem);

    cpu->ConnectBus(bus);
    mem->ConnectBus(bus);

    System* nes = new System(bus, cpu, mem);
    nes->Reset();

    return nes;
}

}
