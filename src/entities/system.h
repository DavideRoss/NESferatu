#pragma once

#include "cpu.h"
#include "memory.h"
#include "bus.h"

class System
{

public:
    System(Cartridge& cartridge);

    void Reset();
    void Execute(const u32 cycles);

public:
    std::unique_ptr<Bus> bus;
    std::unique_ptr<CPU> cpu;
    std::unique_ptr<Memory> mem;
};
