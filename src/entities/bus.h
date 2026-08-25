#pragma once

#include "utils/types.h"

#include "cpu.h"
#include "memory.h"

class Bus
{

public:
    Bus(CPU* in_cpu, Memory* in_memory);

    u8 ReadByte(u16 address);

private:
    CPU* cpu_;
    Memory* memory_;
};
