#pragma once

// #include "utils/types.h"

#include <cstdint>

using u8 = uint8_t;
using u16 = uint16_t;

class CPU
{
public:
    void ConnectBus(Bus* bus);
    void Reset();

public:
    u8 a;      // Accumulator
    u8 x, y;  // GP registers
    u16 pc;    // Program counter
    u8 s;      // Stack pointer
    u8 p;      // Program status

private:
    Bus* bus_ = nullptr;
};
