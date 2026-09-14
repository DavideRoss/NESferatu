#pragma once

#include "utils/types.h"

#include "cartridge.h"
#include "cpu.h"
#include "memory.h"
#include "ppu.h"
#include "apu.h"

class Bus
{

public:
    Bus(CPU& cpu, Memory& memory, PPU& ppu, APU& apu, Cartridge& cartridge);

    u8 CPURead(u16 address);
    void CPUWrite(u16 address, u8 data);

    u8 PPURead(u16 address);
    void PPUWrite(u16 address, u8 data);

private:
    CPU& cpu_;
    Memory& memory_;
    PPU& ppu_;
    APU& apu_;
    Cartridge& cartridge_;
};
