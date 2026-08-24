#pragma once

#include "cpu.h"
#include "memory.h"
#include "bus.h"

class System
{

public:
    System(Bus* in_bus, CPU* in_cpu, Memory* in_memory);

    void Reset();

public:
    Bus* bus;
    CPU* cpu;
    Memory* mem;
};
