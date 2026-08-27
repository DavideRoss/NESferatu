#pragma once

#include "utils/types.h"

#pragma region Definitions

#define ADDRESSING_MODE(name) \
    struct name { \
        static AddressResult GetAddress([[maybe_unused]] CPU& cpu, [[maybe_unused]] u32& cycles); \
    }

#define DEFINE_ADDRESSING_MODE(name) \
    CPU::AddressResult CPU::name::GetAddress([[maybe_unused]] CPU& cpu, [[maybe_unused]] u32& cycles)
    
#define OPERATION(name) \
    struct name { \
        static void Execute([[maybe_unused]] CPU& cpu, [[maybe_unused]] u16 address); \
    }

#define DEFINE_OPERATION(name) \
    void CPU::name::Execute([[maybe_unused]] CPU& cpu, [[maybe_unused]] u16 address)

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
    // Add with Carry
    static constexpr u8 ADC_IM = 0x69;
    static constexpr u8 ADC_ZP = 0x65;
    static constexpr u8 ADC_ZPX = 0x75;
    static constexpr u8 ADC_ABS = 0x6d;
    static constexpr u8 ADC_ABX = 0x7d;
    static constexpr u8 ADC_ABY = 0x79;
    static constexpr u8 ADC_IDX = 0x61;
    static constexpr u8 ADC_IDY = 0x71;

    // Logical AND
    static constexpr u8 AND_IM = 0x29;
    static constexpr u8 AND_ZP = 0x25;
    static constexpr u8 AND_ZPX = 0x35;
    static constexpr u8 AND_ABS = 0x2d;
    static constexpr u8 AND_ABX = 0x3d;
    static constexpr u8 AND_ABY = 0x39;
    static constexpr u8 AND_IDX = 0x21;
    static constexpr u8 AND_IDY = 0x31;

    // Decrement Memory
    static constexpr u8 DEC_ZP = 0xc6;
    static constexpr u8 DEC_ZPX = 0xd6;
    static constexpr u8 DEC_ABS = 0xce;
    static constexpr u8 DEC_ABX = 0xde;

    // Decrement X Registry
    static constexpr u8 DEX_IMP = 0xca;

    // Decrement Y Registry
    static constexpr u8 DEY_IMP = 0x88;

    // Increment Memory
    static constexpr u8 INC_ZP = 0xe6;
    static constexpr u8 INC_ZPX = 0xf6;
    static constexpr u8 INC_ABS = 0xee;
    static constexpr u8 INC_ABX = 0xfe;

    // Increment X Registry
    static constexpr u8 INX_IMP = 0xe8;

    // Increment Y Registry
    static constexpr u8 INY_IMP = 0xc8;

    // Load Accumulator
    static constexpr u8 LDA_IM = 0xa9;
    static constexpr u8 LDA_ZP = 0xa5;
    static constexpr u8 LDA_ZPX = 0xb5;
    static constexpr u8 LDA_ABS = 0xad;
    static constexpr u8 LDA_ABX = 0xbd;
    static constexpr u8 LDA_ABY = 0xb9;
    static constexpr u8 LDA_IDX = 0xa1;
    static constexpr u8 LDA_IDY = 0xb1;

    // Load X Register
    static constexpr u8 LDX_IM = 0xa2;
    static constexpr u8 LDX_ZP = 0xa6;
    static constexpr u8 LDX_ZPY = 0xb6;
    static constexpr u8 LDX_ABS = 0xae;
    static constexpr u8 LDX_ABY = 0xbe;

    // Load Y Register
    static constexpr u8 LDY_IM = 0xa0;
    static constexpr u8 LDY_ZP = 0xa4;
    static constexpr u8 LDY_ZPX = 0xb4;
    static constexpr u8 LDY_ABS = 0xac;
    static constexpr u8 LDY_ABX = 0xbc;

    // Subtract with Carry
    static constexpr u8 SBC_IM = 0xe9;
    static constexpr u8 SBC_ZP = 0xe5;
    static constexpr u8 SBC_ZPX = 0xf5;
    static constexpr u8 SBC_ABS = 0xed;
    static constexpr u8 SBC_ABX = 0xfd;
    static constexpr u8 SBC_ABY = 0xf0;
    static constexpr u8 SBC_IDX = 0xe1;
    static constexpr u8 SBC_IDY = 0xf1;

    // Store Accumulator
    static constexpr u8 STA_ZP = 0x85;
    static constexpr u8 STA_ZPX = 0x95;
    static constexpr u8 STA_ABS = 0x8d;
    static constexpr u8 STA_ABX = 0x9d;
    static constexpr u8 STA_ABY = 0x99;
    static constexpr u8 STA_IDX = 0x81;
    static constexpr u8 STA_IDY = 0x91;

    // Store X Register
    static constexpr u8 STX_ZP = 0x86;
    static constexpr u8 STX_ZPY = 0x96;
    static constexpr u8 STX_ABS = 0x8e;

    // Store X Register
    static constexpr u8 STY_ZP = 0x84;
    static constexpr u8 STY_ZPX = 0x94;
    static constexpr u8 STY_ABS = 0x8c;

    // Transfer Accumulator to X
    static constexpr u8 TAX_IMP = 0xaa;

    // Transfer Accumuator to Y
    static constexpr u8 TAY_IMP = 0xa8;

    // Transfer X to Accumulator
    static constexpr u8 TXA_IMP = 0xba;

    // Transfer Y to Accumulator
    static constexpr u8 TYA_IMP = 0x98;
}

class CPU
{
public:
    void ConnectBus(Bus* bus);
    void Reset();

    u32 Step();

    u8 FetchByte();
    u8 ReadByte(u16 address);
    void WriteByte(u16 address, u8 data);

    u8 GetFlag(StatusFlag flag);
    void SetFlag(StatusFlag flag, bool val);

public:
    u8 a;       // Accumulator
    u8 x, y;    // GP registers
    u16 pc;     // Program counter
    u8 sp;      // Stack pointer
    u8 s;       // Program status

private:
    struct AddressResult
    {
        u16 address;
        bool page_crossed;
    };

    template<
        typename Operation,
        typename AddressingMode,
        bool PageCrossPenalty = false,
        u32 ExtraCycles = 0
    >
    void Execute()
    {
        const AddressResult res = AddressingMode::GetAddress(*this, cycles_);

        cycles_ += ExtraCycles;
        if constexpr(PageCrossPenalty)
        {
            cycles_ += res.page_crossed;
        }

        Operation::Execute(*this, res.address);
    }

#pragma region Addressing Modes

private:
    ADDRESSING_MODE(Implied);
    ADDRESSING_MODE(Immediate);
    ADDRESSING_MODE(ZeroPage);
    ADDRESSING_MODE(ZeroPageX);
    ADDRESSING_MODE(ZeroPageY);
    ADDRESSING_MODE(Absolute);
    ADDRESSING_MODE(AbsoluteX);
    ADDRESSING_MODE(AbsoluteY);
    ADDRESSING_MODE(IndirectX);
    ADDRESSING_MODE(IndirectY);

    OPERATION(ADC);
    OPERATION(AND);
    OPERATION(DEC);
    OPERATION(DEX);
    OPERATION(DEY);
    OPERATION(INC);
    OPERATION(INX);
    OPERATION(INY);
    OPERATION(LDA);
    OPERATION(LDX);
    OPERATION(LDY);
    OPERATION(SBC);
    OPERATION(STA);
    OPERATION(STX);
    OPERATION(STY);
    OPERATION(TAX);
    OPERATION(TAY);
    OPERATION(TXA);
    OPERATION(TYA);

#pragma endregion

private:
    Bus* bus_ = nullptr;
    u32 cycles_ = 0;
};
