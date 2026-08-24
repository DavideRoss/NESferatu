#pragma once

#include "cpu.h"
#include "memory.h"

class Bus
{

public:
    Bus(CPU* in_cpu, Memory* in_memory);

    

private:
    CPU* cpu_;
    Memory* memory_;
};
