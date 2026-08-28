#pragma once

#include "cpu.h"
#include "memory.h"
#include "bus.h"

class System
{

public:
    System();

    void Reset();
    void Execute(const u32 cycles);

public:
    Bus* bus;
    CPU* cpu;
    Memory* mem;
};
