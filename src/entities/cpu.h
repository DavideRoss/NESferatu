#pragma once

#include "utils/types.h"

class Bus;

enum StatusFlag
{
    C = (1 << 0),   // Carry
    Z = (1 << 1),   // Zero
    I = (1 << 2),   // Disable interrupts
    D = (1 << 3),   // Decimal mode
    B = (1 << 4),   // Break
    U = (1 << 5),   // Unused
    V = (1 << 6),   // Overflow
    N = (1 << 7)    // Negative
};

namespace Opcodes
{
    static constexpr u8 LDA_IM = 0xa9;
    static constexpr u8 LDA_ZP = 0xa5;
    static constexpr u8 LDA_ZPX = 0xb5;
}

class CPU
{
public:
    void ConnectBus(Bus* bus);
    void Reset();

    void Execute(const u32 cycles);

    u8 FetchByte();
    u8 PeekByte(u16 address);

    void SetFlag(StatusFlag flag, bool val);

public:
    u8 a;       // Accumulator
    u8 x, y;    // GP registers
    u16 pc;     // Program counter
    u8 sp;      // Stack pointer
    u8 s;       // Program status

private:
    void LDAImmediate();
    void LDAZeroPage();
    void LDAZeroPageX();

private:
    Bus* bus_ = nullptr;
    u32 cycles_ = 0;
};
