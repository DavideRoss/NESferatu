#pragma once

#include "utils/types.h"

#include "cartridge.h"
#include "cpu.h"
#include "memory.h"

class Bus
{

public:
    Bus(CPU& cpu, Memory& memory, Cartridge& cartridge);

    u8 ReadByte(u16 address);
    void WriteByte(u16 address, u8 data);

private:
    CPU& cpu_;
    Memory& memory_;
    Cartridge& cartridge_;
};
