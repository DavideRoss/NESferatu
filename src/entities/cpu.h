#pragma once

#include "utils/types.h"

#pragma region Definitions

#define ADDRESSING_MODE(name) \
    struct name { \
        static u16 GetAddress(CPU& cpu, [[maybe_unused]] u32& cycles); \
    }

#define DEFINE_ADDRESSING_MODE(name) \
    u16 CPU::name::GetAddress(CPU& cpu, [[maybe_unused]] u32& cycles)
    
#define OPERATION(name) \
    struct name { \
        static void Execute(CPU& cpu, u16 address); \
    }

#define DEFINE_OPERATION(name) \
    void CPU::name::Execute(CPU& cpu, u16 address)

#pragma endregion

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
    static constexpr u8 LDA_ABS = 0xad;
    static constexpr u8 LDA_ABX = 0xbd;
    static constexpr u8 LDA_ABY = 0xb9;
}

class CPU
{
public:
    void ConnectBus(Bus* bus);
    void Reset();

    u32 Step();

    u8 FetchByte();
    u8 ReadByte(u16 address);

    void SetFlag(StatusFlag flag, bool val);

public:
    u8 a;       // Accumulator
    u8 x, y;    // GP registers
    u16 pc;     // Program counter
    u8 sp;      // Stack pointer
    u8 s;       // Program status

#pragma region Addressing Modes

private:
    ADDRESSING_MODE(Immediate);
    ADDRESSING_MODE(ZeroPage);
    ADDRESSING_MODE(ZeroPageX);
    ADDRESSING_MODE(Absolute);
    ADDRESSING_MODE(AbsoluteX);
    ADDRESSING_MODE(AbsoluteY);

    OPERATION(LDA);

#pragma endregion

private:
    template<typename Operation, typename AddressingMode>
    void Execute()
    {
        const u16 address = AddressingMode::GetAddress(*this, cycles_);
        Operation::Execute(*this, address);
    }

private:
    Bus* bus_ = nullptr;
    u32 cycles_ = 0;
};
