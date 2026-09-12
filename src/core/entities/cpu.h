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
        static void Execute([[maybe_unused]] CPU& cpu); \
    }

#define DEFINE_OPERATION(name) \
    void CPU::name::Execute([[maybe_unused]] CPU& cpu, [[maybe_unused]] u16 address)

#define DEFINE_OPERATION_NO_ADDRESS(name) \
    void CPU::name::Execute([[maybe_unused]] CPU& cpu)

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

    // Arithmetic Shift Left
    static constexpr u8 ASL_ACC = 0x0a;
    static constexpr u8 ASL_ZP = 0x06;
    static constexpr u8 ASL_ZPX = 0x16;
    static constexpr u8 ASL_ABS = 0x0e;
    static constexpr u8 ASL_ABX = 0x1e;

    // Branch if Carry Clear
    static constexpr u8 BCC_REL = 0x90;
    
    // Branch if Carry Set
    static constexpr u8 BCS_REL = 0xb0;
    
    // Branch if Equal
    static constexpr u8 BEQ_REL = 0xf0;

    // Bit Test
    static constexpr u8 BIT_ZP = 0x24;
    static constexpr u8 BIT_ABS = 0x2c;
    
    // Branch if Minus
    static constexpr u8 BMI_REL = 0x30;

    // Branch if Not Equal
    static constexpr u8 BNE_REL = 0xd0;
    
    // Branch if Positive
    static constexpr u8 BPL_REL = 0x10;
    
    // Force Interrupt
    static constexpr u8 BRK_IMP = 0x00;
    
    // Branch of Overflow Clear
    static constexpr u8 BVC_REL = 0x50;

    // Branch of Overflow Set
    static constexpr u8 BVS_REL = 0x70;

    // Compare
    static constexpr u8 CMP_IM = 0xc9;
    static constexpr u8 CMP_ZP = 0xc5;
    static constexpr u8 CMP_ZPX = 0xd5;
    static constexpr u8 CMP_ABS = 0xcd;
    static constexpr u8 CMP_ABX = 0xdd;
    static constexpr u8 CMP_ABY = 0xd9;
    static constexpr u8 CMP_IDX = 0xc1;
    static constexpr u8 CMP_IDY = 0xd1;

    // Compare X Register
    static constexpr u8 CPX_IM = 0xe0;
    static constexpr u8 CPX_ZP = 0xe4;
    static constexpr u8 CPX_ABS = 0xec;

    // Compare Y Register
    static constexpr u8 CPY_IM = 0xc0;
    static constexpr u8 CPY_ZP = 0xc4;
    static constexpr u8 CPY_ABS = 0xcc;

    // Decrement Memory
    static constexpr u8 DEC_ZP = 0xc6;
    static constexpr u8 DEC_ZPX = 0xd6;
    static constexpr u8 DEC_ABS = 0xce;
    static constexpr u8 DEC_ABX = 0xde;

    // Decrement X Registry
    static constexpr u8 DEX_IMP = 0xca;

    // Decrement Y Registry
    static constexpr u8 DEY_IMP = 0x88;

    // Exclusive OR
    static constexpr u8 EOR_IM = 0x49;
    static constexpr u8 EOR_ZP = 0x45;
    static constexpr u8 EOR_ZPX = 0x55;
    static constexpr u8 EOR_ABS = 0x4d;
    static constexpr u8 EOR_ABX = 0x5d;
    static constexpr u8 EOR_ABY = 0x59;
    static constexpr u8 EOR_IDX = 0x41;
    static constexpr u8 EOR_IDY = 0x51;

    // Increment Memory
    static constexpr u8 INC_ZP = 0xe6;
    static constexpr u8 INC_ZPX = 0xf6;
    static constexpr u8 INC_ABS = 0xee;
    static constexpr u8 INC_ABX = 0xfe;

    // Increment X Registry
    static constexpr u8 INX_IMP = 0xe8;

    // Increment Y Registry
    static constexpr u8 INY_IMP = 0xc8;
    
    // Jump
    static constexpr u8 JMP_ABS = 0x4c;
    static constexpr u8 JMP_IND = 0x6c;
    
    // Jump to Subroutine
    static constexpr u8 JSR_ABS = 0x20;

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

    // Logical Shift Right
    static constexpr u8 LSR_ACC = 0x4a;
    static constexpr u8 LSR_ZP = 0x46;
    static constexpr u8 LSR_ZPX = 0x56;
    static constexpr u8 LSR_ABS = 0x4e;
    static constexpr u8 LSR_ABX = 0x5e;

    // Logical Inclusive OR
    static constexpr u8 ORA_IM = 0x09;
    static constexpr u8 ORA_ZP = 0x05;
    static constexpr u8 ORA_ZPX = 0x15;
    static constexpr u8 ORA_ABS = 0x0d;
    static constexpr u8 ORA_ABX = 0x1d;
    static constexpr u8 ORA_ABY = 0x19;
    static constexpr u8 ORA_IDX = 0x01;
    static constexpr u8 ORA_IDY = 0x11;

    // Push Accumulator
    static constexpr u8 PHA_IMP = 0x48;
    
    // Push Processor Status
    static constexpr u8 PHP_IMP = 0x08;

    // Pull Accumulator
    static constexpr u8 PLA_IMP = 0x68;

    // Pull Processor Status
    static constexpr u8 PLP_IMP = 0x28;

    // Rotate Left
    static constexpr u8 ROL_ACC = 0x2a;
    static constexpr u8 ROL_ZP = 0x26;
    static constexpr u8 ROL_ZPX = 0x36;
    static constexpr u8 ROL_ABS = 0x2e;
    static constexpr u8 ROL_ABX = 0x3e;

    // Rotate Right
    static constexpr u8 ROR_ACC = 0x6a;
    static constexpr u8 ROR_ZP = 0x66;
    static constexpr u8 ROR_ZPX = 0x76;
    static constexpr u8 ROR_ABS = 0x6e;
    static constexpr u8 ROR_ABX = 0x7e;

    // Return from Interrupt
    static constexpr u8 RTI_IMP = 0x40;
    
    // Return from Subroutine
    static constexpr u8 RTS_IMP = 0x60;

    // Subtract with Carry
    static constexpr u8 SBC_IM = 0xe9;
    static constexpr u8 SBC_ZP = 0xe5;
    static constexpr u8 SBC_ZPX = 0xf5;
    static constexpr u8 SBC_ABS = 0xed;
    static constexpr u8 SBC_ABX = 0xfd;
    static constexpr u8 SBC_ABY = 0xf9;
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
    
    // Transfer Stack Pointer to X
    static constexpr u8 TSX_IMP = 0xba;

    // Transfer X to Accumulator
    static constexpr u8 TXA_IMP = 0x8a;

    // Transfer X to Stack Pointer
    static constexpr u8 TXS_IMP = 0x9a;

    // Transfer Y to Accumulator
    static constexpr u8 TYA_IMP = 0x98;
}

class CPU
{
public:
    void ConnectBus(Bus* bus);
    void Reset();

    u32 Step();

    u8 PeekByte();
    u8 FetchByte();
    u8 ReadByte(u16 address);
    void WriteByte(u16 address, u8 data);

    u8 GetFlag(StatusFlag flag);
    void SetFlag(StatusFlag flag, bool val);

    void PushToStack(const u8 data);
    u8 PullFromStack();

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
        u32 ExtraCycles = 0
    >
    void Execute()
    {
        cycles_ += ExtraCycles;
        Operation::Execute(*this);
    }

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

private:
    // TODO: cleanup implied addressing mode after using NO_ADDRESS macro
    ADDRESSING_MODE(Implied);
    ADDRESSING_MODE(Immediate);
    ADDRESSING_MODE(Relative);
    ADDRESSING_MODE(ZeroPage);
    ADDRESSING_MODE(ZeroPageX);
    ADDRESSING_MODE(ZeroPageY);
    ADDRESSING_MODE(Absolute);
    ADDRESSING_MODE(AbsoluteX);
    ADDRESSING_MODE(AbsoluteY);
    ADDRESSING_MODE(Indirect);
    ADDRESSING_MODE(IndirectX);
    ADDRESSING_MODE(IndirectY);

    OPERATION(ADC);
    OPERATION(AND);
    OPERATION(ASL);
    OPERATION(BCC);
    OPERATION(BCS);
    OPERATION(BEQ);
    OPERATION(BIT);
    OPERATION(BMI);
    OPERATION(BNE);
    OPERATION(BPL);
    OPERATION(BRK);
    OPERATION(BVC);
    OPERATION(BVS);
    OPERATION(CMP);
    OPERATION(CPX);
    OPERATION(CPY);
    OPERATION(DEC);
    OPERATION(DEX);
    OPERATION(DEY);
    OPERATION(EOR);
    OPERATION(INC);
    OPERATION(INX);
    OPERATION(INY);
    OPERATION(JMP);
    OPERATION(JSR);
    OPERATION(LDA);
    OPERATION(LDX);
    OPERATION(LDY);
    OPERATION(LSR);
    OPERATION(ORA);
    OPERATION(PHA);
    OPERATION(PHP);
    OPERATION(PLA);
    OPERATION(PLP);
    OPERATION(ROL);
    OPERATION(ROR);
    OPERATION(RTI);
    OPERATION(RTS);
    OPERATION(SBC);
    OPERATION(STA);
    OPERATION(STX);
    OPERATION(STY);
    OPERATION(TAX);
    OPERATION(TAY);
    OPERATION(TSX);
    OPERATION(TXA);
    OPERATION(TXS);
    OPERATION(TYA);

private:
    Bus* bus_ = nullptr;
    u32 cycles_ = 0;
};
