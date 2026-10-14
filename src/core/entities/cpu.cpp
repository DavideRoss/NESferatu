#include "cpu.h"

#include "bus.h"
#include "utils/log.h"

#define EXECUTE_OPCODE(opcode, operation, addressing_mode, extra_cycles) \
    case Opcodes::opcode: Execute<operation, addressing_mode, extra_cycles>(); break;

#define EXECUTE_OPCODE_NO_ADDRESS(opcode, operation, extra_cycles) \
    case Opcodes::opcode: Execute<operation, extra_cycles>(); break;

void CPU::ConnectBus(Bus *bus)
{
    bus_ = bus;
}

void CPU::Reset()
{
    a = x = y = 0x0;
    pc = 0xfffc;
    sp = 0xfd;
    s = 0x20;

    total_cycles = 7;

    // Read reset vector
    u8 reset_low = ReadByte(0xfffc);
    u8 reset_high = ReadByte(0xfffd);
    pc = (reset_high << 8) | reset_low;

}

u32 CPU::Step()
{
    cycles_ = 0;

    if (nmi_pending_)
    {
        nmi_pending_ = false;

        PushToStack(pc >> 8);
        PushToStack(pc & 0xff);
        PushToStack(s & ~StatusFlag::B);

        SetFlag(StatusFlag::I, true);

        pc = static_cast<u16>((ReadByte(0xfffb) << 8) | ReadByte(0xfffa));
        cycles_ += 2;

        total_cycles += cycles_;
        return cycles_;
    }

    if (irq_pending_ && !GetFlag(StatusFlag::I))
    {
        irq_pending_ = false;

        PushToStack(pc >> 8);
        PushToStack(pc & 0xff);
        PushToStack(s & ~StatusFlag::B);

        SetFlag(StatusFlag::I, true);

        pc = static_cast<u16>((ReadByte(0xffff) << 8) | ReadByte(0xfffe));
        cycles_ += 2;

        total_cycles += cycles_;
        return cycles_;
    }
    
    // TODO: use DEFINE_OPERATION_NO_ADDRESS for opcodes with implied operands
    // TODO: cleanup implied addressing mode afterwards
    u16 prev_pc = pc;
    u8 instruction = FetchByte();

    // NESFERATU_LOG(
    //     "{:04X}\t{:0X}\t\t\tA:{:02X} X:{:02X} Y:{:02X} SP:{:02X} PPU:0, 0 CYC:{}",
    //     prev_pc, instruction, a, x, y, sp, total_cycles
    // );
    
    Debug({
        .event = DebugEventType::CPUExecute,
        .pc = prev_pc,
        .instruction = instruction
    });

    switch (instruction)
    {
        // Add with Carry
        EXECUTE_OPCODE(ADC_IM,  ADC, Immediate, 0)
        EXECUTE_OPCODE(ADC_ZP,  ADC, ZeroPage,  0)
        EXECUTE_OPCODE(ADC_ZPX, ADC, ZeroPageX, 0)
        EXECUTE_OPCODE(ADC_ABS, ADC, Absolute,  0)
        EXECUTE_OPCODE(ADC_ABX, ADC, AbsoluteX, 0)
        EXECUTE_OPCODE(ADC_ABY, ADC, AbsoluteY, 0)
        EXECUTE_OPCODE(ADC_IDX, ADC, IndirectX, 0)
        EXECUTE_OPCODE(ADC_IDY, ADC, IndirectY, 0)

        // Logical AND
        EXECUTE_OPCODE(AND_IM,  AND, Immediate, 0)
        EXECUTE_OPCODE(AND_ZP,  AND, ZeroPage,  0)
        EXECUTE_OPCODE(AND_ZPX, AND, ZeroPageX, 0)
        EXECUTE_OPCODE(AND_ABS, AND, Absolute,  0)
        EXECUTE_OPCODE(AND_ABX, AND, AbsoluteX, 0)
        EXECUTE_OPCODE(AND_ABY, AND, AbsoluteY, 0)
        EXECUTE_OPCODE(AND_IDX, AND, IndirectX, 0)
        EXECUTE_OPCODE(AND_IDY, AND, IndirectY, 0)

        // Arithmetic Shift Left
        EXECUTE_OPCODE_NO_ADDRESS(ASL_ACC, ASL, 1)
        EXECUTE_OPCODE(ASL_ZP,  ASL, ZeroPage,  1)
        EXECUTE_OPCODE(ASL_ZPX, ASL, ZeroPageX, 1)
        EXECUTE_OPCODE(ASL_ABS, ASL, Absolute,  1)
        EXECUTE_OPCODE(ASL_ABX, ASL, AbsoluteX, 2)
        
        // Branch if Carry Clear
        EXECUTE_OPCODE(BCC_REL, BCC, Relative,  0)
        
        // Branch if Carry Set
        EXECUTE_OPCODE(BCS_REL, BCS, Relative,  0)

        // Branch if Equal
        EXECUTE_OPCODE(BEQ_REL, BEQ, Relative,  0)

        // Branch if Minus
        EXECUTE_OPCODE(BMI_REL, BMI, Relative,  0)
        
        // Bit Test
        EXECUTE_OPCODE(BIT_ZP,  BIT, ZeroPage,  0)
        EXECUTE_OPCODE(BIT_ABS, BIT, Absolute,  0)
        
        // Branch if Not Equal
        EXECUTE_OPCODE(BNE_REL, BNE, Relative,  0)

        // Branch if Positive
        EXECUTE_OPCODE(BPL_REL, BPL, Relative,  0)

        // Force Interrupt
        EXECUTE_OPCODE(BRK_IMP, BRK, Implied,   1)

        // Branch of Overflow Clear
        EXECUTE_OPCODE(BVC_REL, BVC, Relative,  0)

        // Branch of Overflow Set
        EXECUTE_OPCODE(BVS_REL, BVS, Relative,  0)

        // Clear Carry
        EXECUTE_OPCODE_NO_ADDRESS(CLC_IMP, CLC, 1)

        // Clear Decimal Mode
        EXECUTE_OPCODE_NO_ADDRESS(CLD_IMP, CLD, 1)

        // Clear Interrupt Disable
        EXECUTE_OPCODE_NO_ADDRESS(CLI_IMP, CLI, 1)

        // Clear Overflow
        EXECUTE_OPCODE_NO_ADDRESS(CLV_IMP, CLV, 1)

        // Compare
        EXECUTE_OPCODE(CMP_IM,  CMP, Immediate, 0)
        EXECUTE_OPCODE(CMP_ZP,  CMP, ZeroPage,  0)
        EXECUTE_OPCODE(CMP_ZPX, CMP, ZeroPageX, 0)
        EXECUTE_OPCODE(CMP_ABS, CMP, Absolute,  0)
        EXECUTE_OPCODE(CMP_ABX, CMP, AbsoluteX, 0)
        EXECUTE_OPCODE(CMP_ABY, CMP, AbsoluteY, 0)
        EXECUTE_OPCODE(CMP_IDX, CMP, IndirectX, 0)
        EXECUTE_OPCODE(CMP_IDY, CMP, IndirectY, 0)

        // Compare X Register
        EXECUTE_OPCODE(CPX_IM,  CPX, Immediate, 0)
        EXECUTE_OPCODE(CPX_ZP,  CPX, ZeroPage,  0)
        EXECUTE_OPCODE(CPX_ABS, CPX, Absolute,  0)

        // Compare Y Register
        EXECUTE_OPCODE(CPY_IM,  CPY, Immediate, 0)
        EXECUTE_OPCODE(CPY_ZP,  CPY, ZeroPage,  0)
        EXECUTE_OPCODE(CPY_ABS, CPY, Absolute,  0)

        // Decrement Memory
        EXECUTE_OPCODE(DEC_ZP,  DEC, ZeroPage,  1)
        EXECUTE_OPCODE(DEC_ZPX, DEC, ZeroPageX, 1)
        EXECUTE_OPCODE(DEC_ABS, DEC, Absolute,  1)
        EXECUTE_OPCODE(DEC_ABX, DEC, AbsoluteX, 2)

        // Decrement X Register
        EXECUTE_OPCODE(DEX_IMP, DEX, Implied,   1)

        // Decrement Y Register
        EXECUTE_OPCODE(DEY_IMP, DEY, Implied,   1)

        // Exclusive OR
        EXECUTE_OPCODE(EOR_IM,  EOR, Immediate, 0)
        EXECUTE_OPCODE(EOR_ZP,  EOR, ZeroPage,  0)
        EXECUTE_OPCODE(EOR_ZPX, EOR, ZeroPageX, 0)
        EXECUTE_OPCODE(EOR_ABS, EOR, Absolute,  0)
        EXECUTE_OPCODE(EOR_ABX, EOR, AbsoluteX, 0)
        EXECUTE_OPCODE(EOR_ABY, EOR, AbsoluteY, 0)
        EXECUTE_OPCODE(EOR_IDX, EOR, IndirectX, 0)
        EXECUTE_OPCODE(EOR_IDY, EOR, IndirectY, 0)

        // Increment Memory
        EXECUTE_OPCODE(INC_ZP,  INC, ZeroPage,  1)
        EXECUTE_OPCODE(INC_ZPX, INC, ZeroPageX, 1)
        EXECUTE_OPCODE(INC_ABS, INC, Absolute,  1)
        EXECUTE_OPCODE(INC_ABX, INC, AbsoluteX, 2)

        // Increment X Register
        EXECUTE_OPCODE(INX_IMP, INX, Implied,   1)

        // Increment Y Register
        EXECUTE_OPCODE(INY_IMP, INY, Implied,   1)
        
        // Jump
        EXECUTE_OPCODE(JMP_ABS, JMP, Absolute,  0)
        EXECUTE_OPCODE(JMP_IND, JMP, Indirect,  0)
        
        // Jump to Subroutine
        EXECUTE_OPCODE(JSR_ABS, JSR, Absolute,  1)

        // Load Accumulator
        EXECUTE_OPCODE(LDA_IM,  LDA, Immediate, 0)
        EXECUTE_OPCODE(LDA_ZP,  LDA, ZeroPage,  0)
        EXECUTE_OPCODE(LDA_ZPX, LDA, ZeroPageX, 0)
        EXECUTE_OPCODE(LDA_ABS, LDA, Absolute,  0)
        EXECUTE_OPCODE(LDA_ABX, LDA, AbsoluteX, 0)
        EXECUTE_OPCODE(LDA_ABY, LDA, AbsoluteY, 0)
        EXECUTE_OPCODE(LDA_IDX, LDA, IndirectX, 0)
        EXECUTE_OPCODE(LDA_IDY, LDA, IndirectY, 0)

        // Load X Register
        EXECUTE_OPCODE(LDX_IM,  LDX, Immediate, 0)
        EXECUTE_OPCODE(LDX_ZP,  LDX, ZeroPage,  0)
        EXECUTE_OPCODE(LDX_ZPY, LDX, ZeroPageY, 0)
        EXECUTE_OPCODE(LDX_ABS, LDX, Absolute,  0)
        EXECUTE_OPCODE(LDX_ABY, LDX, AbsoluteY, 0)

        // Load Y Register
        EXECUTE_OPCODE(LDY_IM,  LDY, Immediate, 0)
        EXECUTE_OPCODE(LDY_ZP,  LDY, ZeroPage,  0)
        EXECUTE_OPCODE(LDY_ZPX, LDY, ZeroPageX, 0)
        EXECUTE_OPCODE(LDY_ABS, LDY, Absolute,  0)
        EXECUTE_OPCODE(LDY_ABX, LDY, AbsoluteX, 0)

        // Logical Shift Right
        EXECUTE_OPCODE_NO_ADDRESS(LSR_ACC, LSR, 1)
        EXECUTE_OPCODE(LSR_ZP,  LSR, ZeroPage,  1)
        EXECUTE_OPCODE(LSR_ZPX, LSR, ZeroPageX, 1)
        EXECUTE_OPCODE(LSR_ABS, LSR, Absolute,  1)
        EXECUTE_OPCODE(LSR_ABX, LSR, AbsoluteX, 2)

        // Logical Inclusive OR
        EXECUTE_OPCODE(ORA_IM,  ORA, Immediate, 0)
        EXECUTE_OPCODE(ORA_ZP,  ORA, ZeroPage,  0)
        EXECUTE_OPCODE(ORA_ZPX, ORA, ZeroPageX, 0)
        EXECUTE_OPCODE(ORA_ABS, ORA, Absolute,  0)
        EXECUTE_OPCODE(ORA_ABX, ORA, AbsoluteX, 0)
        EXECUTE_OPCODE(ORA_ABY, ORA, AbsoluteY, 0)
        EXECUTE_OPCODE(ORA_IDX, ORA, IndirectX, 0)
        EXECUTE_OPCODE(ORA_IDY, ORA, IndirectY, 0)

        // No Operation
        EXECUTE_OPCODE_NO_ADDRESS(NOP_IMP, NOP, 1)

        // Push Accumulator
        EXECUTE_OPCODE_NO_ADDRESS(PHA_IMP, PHA, 1)

        // Push Processor Status
        EXECUTE_OPCODE_NO_ADDRESS(PHP_IMP, PHP, 1)

        // Pull Accumulator
        EXECUTE_OPCODE_NO_ADDRESS(PLA_IMP, PLA, 2)

        // Pull Processor Status
        EXECUTE_OPCODE_NO_ADDRESS(PLP_IMP, PLP, 2)

        // Rotate Left
        EXECUTE_OPCODE_NO_ADDRESS(ROL_ACC, ROL, 1)
        EXECUTE_OPCODE(ROL_ZP,  ROL, ZeroPage,  1)
        EXECUTE_OPCODE(ROL_ZPX, ROL, ZeroPageX, 1)
        EXECUTE_OPCODE(ROL_ABS, ROL, Absolute,  1)
        EXECUTE_OPCODE(ROL_ABX, ROL, AbsoluteX, 2)

        // Rotate Right
        EXECUTE_OPCODE_NO_ADDRESS(ROR_ACC, ROR, 1)
        EXECUTE_OPCODE(ROR_ZP,  ROR, ZeroPage,  1)
        EXECUTE_OPCODE(ROR_ZPX, ROR, ZeroPageX, 1)
        EXECUTE_OPCODE(ROR_ABS, ROR, Absolute,  1)
        EXECUTE_OPCODE(ROR_ABX, ROR, AbsoluteX, 2)

        // Return from Interrupt
        EXECUTE_OPCODE(RTI_IMP, RTI, Implied,   2)

        // Return from Subroutine
        EXECUTE_OPCODE(RTS_IMP, RTS, Implied,   3)
        
        // Subtract with Carry
        EXECUTE_OPCODE(SBC_IM,  SBC, Immediate, 0)
        EXECUTE_OPCODE(SBC_ZP,  SBC, ZeroPage,  0)
        EXECUTE_OPCODE(SBC_ZPX, SBC, ZeroPageX, 0)
        EXECUTE_OPCODE(SBC_ABS, SBC, Absolute,  0)
        EXECUTE_OPCODE(SBC_ABX, SBC, AbsoluteX, 0)
        EXECUTE_OPCODE(SBC_ABY, SBC, AbsoluteY, 0)
        EXECUTE_OPCODE(SBC_IDX, SBC, IndirectX, 0)
        EXECUTE_OPCODE(SBC_IDY, SBC, IndirectY, 0)

        // Set Carry
        EXECUTE_OPCODE_NO_ADDRESS(SEC_IMP, SEC, 1)

        // Set Decimal Mode
        EXECUTE_OPCODE_NO_ADDRESS(SED_IMP, SED, 1)

        // Set Interrupt Disable
        EXECUTE_OPCODE_NO_ADDRESS(SEI_IMP, SEI, 1)

        // Store Accumulator
        EXECUTE_OPCODE(STA_ZP,  STA, ZeroPage,  0)
        EXECUTE_OPCODE(STA_ZPX, STA, ZeroPageX, 0)
        EXECUTE_OPCODE(STA_ABS, STA, Absolute,  0)
        EXECUTE_OPCODE(STA_ABX, STA, AbsoluteX, 1)
        EXECUTE_OPCODE(STA_ABY, STA, AbsoluteY, 1)
        EXECUTE_OPCODE(STA_IDX, STA, IndirectX, 0)
        EXECUTE_OPCODE(STA_IDY, STA, IndirectY, 1)

        // Store X Register
        EXECUTE_OPCODE(STX_ZP,  STX, ZeroPage,  0)
        EXECUTE_OPCODE(STX_ZPY, STX, ZeroPageY, 0)
        EXECUTE_OPCODE(STX_ABS, STX, Absolute,  0)

        // Store Y Register
        EXECUTE_OPCODE(STY_ZP,  STY, ZeroPage,  0)
        EXECUTE_OPCODE(STY_ZPX, STY, ZeroPageX, 0)
        EXECUTE_OPCODE(STY_ABS, STY, Absolute,  0)

        // Transfer Accumulator to X
        EXECUTE_OPCODE(TAX_IMP, TAX, Implied,   1)

        // Transfer Accumulator to Y
        EXECUTE_OPCODE(TAY_IMP, TAY, Implied,   1)

        // Transfer Stack Pointer to X
        EXECUTE_OPCODE_NO_ADDRESS(TSX_IMP, TSX, 1)
        
        // Transfer X to Accumulator
        EXECUTE_OPCODE(TXA_IMP, TXA, Implied,   1)

        // Transfer X to Stack Pointer
        EXECUTE_OPCODE_NO_ADDRESS(TXS_IMP, TXS, 1)

        // Transfer Y to Accumulator
        EXECUTE_OPCODE(TYA_IMP, TYA, Implied,   1)

        default: NESFERATU_LOG("Illegal opcode: 0x{:02x} address: 0x{:04x}", instruction, pc);
    }

    total_cycles += cycles_;
    return cycles_;
}

u8 CPU::PeekByte()
{
    return bus_->CPURead(pc);
}

u8 CPU::FetchByte()
{
    const u8 data = bus_->CPURead(pc);
    pc++;
    cycles_++;
    return data;
}

u8 CPU::ReadByte(u16 address)
{
    const u8 data = bus_->CPURead(address);
    cycles_++;
    return data;
}

void CPU::WriteByte(u16 address, u8 data)
{
    bus_->CPUWrite(address, data);
    cycles_++;
}

u8 CPU::GetFlag(StatusFlag flag)
{
    return ((s & flag) > 0) ? 0x1 : 0x0;
}

void CPU::SetFlag(StatusFlag flag, bool val)
{
    s = val ? s | flag : s & ~flag;
}

void CPU::PushToStack(const u8 data)
{
    bus_->CPUWrite(0x0100 | sp, data);
    --sp;
    cycles_++;
}

u8 CPU::PullFromStack()
{
    ++sp;
    const u8 data = bus_->CPURead(0x0100 | sp);
    cycles_++;
    return data;
}

#pragma region Addressing Modes

DEFINE_ADDRESSING_MODE(Implied)
{
    return {};
}

DEFINE_ADDRESSING_MODE(Immediate)
{
    return cpu.pc++;
}

DEFINE_ADDRESSING_MODE(Relative)
{
    const i8 offset = static_cast<i8>(cpu.FetchByte());
    const u16 target = cpu.pc + offset;
    const bool page_cross = (cpu.pc & 0xff00) != (target & 0xff00);

    return target;
}

DEFINE_ADDRESSING_MODE(ZeroPage)
{
    return cpu.FetchByte();
}

DEFINE_ADDRESSING_MODE(ZeroPageX)
{
    u8 zero_page = cpu.FetchByte();
    zero_page = (zero_page + cpu.x) & 0xff;
    cycles++;

    return zero_page;
}

DEFINE_ADDRESSING_MODE(ZeroPageY)
{
    u8 zero_page = cpu.FetchByte();
    zero_page = (zero_page + cpu.y) & 0xff;
    cycles++;

    return zero_page;
}

DEFINE_ADDRESSING_MODE(Absolute)
{
    const u8 addr_low = cpu.FetchByte();
    const u8 addr_high = cpu.FetchByte();

    return static_cast<u16>((addr_high << 8) | addr_low);
}

DEFINE_ADDRESSING_MODE(AbsoluteX)
{
    const u8 addr_low = cpu.FetchByte();
    const u8 addr_high = cpu.FetchByte();

    const u16 base = static_cast<u16>((addr_high << 8) | addr_low);
    const u16 addr = base + cpu.x;
    const bool page_crossed = (addr & 0xff00) != (addr_high << 8);

    if (page_crossed)
    {
        const u16 dummy_address = (base & 0xff00) | (addr & 0x00ff);
        cpu.ReadByte(dummy_address);
    }

    return addr;
}

DEFINE_ADDRESSING_MODE(AbsoluteY)
{
    const u8 addr_low = cpu.FetchByte();
    const u8 addr_high = cpu.FetchByte();

    const u16 base = static_cast<u16>((addr_high << 8) | addr_low);
    const u16 addr = base + cpu.y;
    const bool page_crossed = (addr & 0xff00) != (addr_high << 8);

    if (page_crossed)
    {
        const u16 dummy_address = (base & 0xff00) | (addr & 0x00ff);
        cpu.ReadByte(dummy_address);
    }

    return addr;
}

// NOTE: describe 6502 address bug behaviour
DEFINE_ADDRESSING_MODE(Indirect)
{
    const u8 target_low = cpu.FetchByte();
    const u8 target_high = cpu.FetchByte();
    const u16 target = static_cast<u16>((target_high << 8) | target_low);

    const u8 addr_low = cpu.ReadByte(target);
    const u8 addr_high = cpu.ReadByte((target & 0xff00) | ((target + 1) & 0x00ff));

    return static_cast<u16>((addr_high << 8) | addr_low);
}

DEFINE_ADDRESSING_MODE(IndirectX)
{
    const u16 zero_page = cpu.FetchByte();
    const u16 addr_low = cpu.ReadByte((zero_page + cpu.x) & 0x00ff);
    const u16 addr_high = cpu.ReadByte((zero_page + cpu.x + 1) & 0x00ff);
    cycles++;

    return static_cast<u16>((addr_high << 8) | addr_low);
}

DEFINE_ADDRESSING_MODE(IndirectY)
{
    const u16 zero_page = cpu.FetchByte();
    const u16 addr_low = cpu.ReadByte(zero_page & 0x00ff);
    const u16 addr_high = cpu.ReadByte((zero_page + 1) & 0x00ff);

    const u16 base = (addr_high << 8) | addr_low;
    const u16 addr = base + cpu.y;
    const bool page_crossed = (addr & 0xff00) != (addr_high << 8);

    if (page_crossed)
    {
        const u16 dummy_address = (base & 0xff00) | (addr & 0x00ff);
        cpu.ReadByte(dummy_address);
    }

    return addr;
}

#pragma endregion

#pragma region Operations

DEFINE_OPERATION(ADC)
{
    u8 data = cpu.ReadByte(address);
    u16 temp = cpu.a + data + cpu.GetFlag(StatusFlag::C);

    cpu.SetFlag(StatusFlag::C, temp > 255);
    cpu.SetFlag(StatusFlag::Z, (temp & 0x00ff) == 0);
    cpu.SetFlag(StatusFlag::V, ((~(cpu.a ^ data) & (cpu.a ^ temp)) & 0x0080) != 0);
    cpu.SetFlag(StatusFlag::N, temp & 0x80);

    cpu.a = temp & 0x00ff;
}

DEFINE_OPERATION(AND)
{
    u8 data = cpu.ReadByte(address);

    cpu.a &= data;

    cpu.SetFlag(StatusFlag::Z, cpu.a == 0);
    cpu.SetFlag(StatusFlag::N, cpu.a & 0x80);
}

DEFINE_OPERATION(ASL)
{
    u8 data = cpu.ReadByte(address);
    const u8 carry = data >> 7;
    data = (data << 1) & 0x00ff;
    cpu.WriteByte(address, data);

    cpu.SetFlag(StatusFlag::C, carry);
    cpu.SetFlag(StatusFlag::Z, data == 0);
    cpu.SetFlag(StatusFlag::N, data & 0x80);
}

DEFINE_OPERATION_NO_ADDRESS(ASL)
{
    u8 carry = cpu.a >> 7;
    cpu.a = (cpu.a << 1) & 0x00ff;

    cpu.SetFlag(StatusFlag::C, carry);
    cpu.SetFlag(StatusFlag::Z, cpu.a == 0);
    cpu.SetFlag(StatusFlag::N, cpu.a & 0x80);
}

DEFINE_OPERATION(BCC)
{
    if (!cpu.GetFlag(StatusFlag::C))
    {
        const u16 old_pc = cpu.pc;
        cpu.pc = address;
        ++cpu.cycles_;

        // Check page crossing
        if ((old_pc & 0xff00) != (cpu.pc & 0xff00))
        {
            ++cpu.cycles_;
        }
    }
}

DEFINE_OPERATION(BCS)
{
    if (cpu.GetFlag(StatusFlag::C))
    {
        const u16 old_pc = cpu.pc;
        cpu.pc = address;
        ++cpu.cycles_;

        // Check page crossing
        if ((old_pc & 0xff00) != (cpu.pc & 0xff00))
        {
            ++cpu.cycles_;
        }  
    }
}

DEFINE_OPERATION(BEQ)
{
    if (cpu.GetFlag(StatusFlag::Z))
    {
        const u16 old_pc = cpu.pc;
        cpu.pc = address;
        ++cpu.cycles_;

        // Check page crossing
        if ((old_pc & 0xff00) != (cpu.pc & 0xff00))
        {
            ++cpu.cycles_;
        }
    }
}

DEFINE_OPERATION(BMI)
{
    if (cpu.GetFlag(StatusFlag::N))
    {
        const u16 old_pc = cpu.pc;
        cpu.pc = address;
        ++cpu.cycles_;

        // Check page crossing
        if ((old_pc & 0xff00) != (cpu.pc & 0xff00))
        {
            ++cpu.cycles_;
        }
    }
}

DEFINE_OPERATION(BIT)
{
    u8 data = cpu.ReadByte(address);

    cpu.SetFlag(StatusFlag::Z, !(data & cpu.a));
    cpu.SetFlag(StatusFlag::V, data & 0x40);
    cpu.SetFlag(StatusFlag::N, data & 0x80);
}

DEFINE_OPERATION(BNE)
{
    if (!cpu.GetFlag(StatusFlag::Z))
    {
        const u16 old_pc = cpu.pc;
        cpu.pc = address;
        ++cpu.cycles_;

        // Check page crossing
        if ((old_pc & 0xff00) != (cpu.pc & 0xff00))
        {
            ++cpu.cycles_;
        }
    }
}

DEFINE_OPERATION(BPL)
{
    if (!cpu.GetFlag(StatusFlag::N))
    {
        const u16 old_pc = cpu.pc;
        cpu.pc = address;
        ++cpu.cycles_;

        // Check page crossing
        if ((old_pc & 0xff00) != (cpu.pc & 0xff00))
        {
            ++cpu.cycles_;
        }
    }
}

DEFINE_OPERATION(BRK)
{
    cpu.PushToStack((cpu.pc + 1) >> 8);
    cpu.PushToStack((cpu.pc + 1) & 0x00ff);
    cpu.PushToStack(cpu.s | StatusFlag::B | StatusFlag::U);

    cpu.SetFlag(StatusFlag::I, true);

    const u8 addr_low = cpu.ReadByte(0xfffe);
    const u8 addr_high = cpu.ReadByte(0xffff);
    cpu.pc = static_cast<u16>((addr_high << 8) | addr_low);
}

DEFINE_OPERATION(BVC)
{
    if (!cpu.GetFlag(StatusFlag::V))
    {
        const u16 old_pc = cpu.pc;
        cpu.pc = address;
        ++cpu.cycles_;

        // Check page crossing
        if ((old_pc & 0xff00) != (cpu.pc & 0xff00))
        {
            ++cpu.cycles_;
        }
    }
}

DEFINE_OPERATION(BVS)
{
    if (cpu.GetFlag(StatusFlag::V))
    {
        const u16 old_pc = cpu.pc;
        cpu.pc = address;
        ++cpu.cycles_;

        // Check page crossing
        if ((old_pc & 0xff00) != (cpu.pc & 0xff00))
        {
            ++cpu.cycles_;
        }
    }
}

DEFINE_OPERATION_NO_ADDRESS(CLC)
{
    cpu.SetFlag(StatusFlag::C, false);
}

DEFINE_OPERATION_NO_ADDRESS(CLD)
{
    cpu.SetFlag(StatusFlag::D, false);
}

// TODO: the I flag should be set 1 instruction later
// Check if it can cause issues
DEFINE_OPERATION_NO_ADDRESS(CLI)
{
    cpu.SetFlag(StatusFlag::I, false);
}

DEFINE_OPERATION_NO_ADDRESS(CLV)
{
    cpu.SetFlag(StatusFlag::V, false);
}

DEFINE_OPERATION(CMP)
{
    const u8 data = cpu.ReadByte(address);
    const u8 result = cpu.a - data;

    cpu.SetFlag(StatusFlag::C, cpu.a >= data);
    cpu.SetFlag(StatusFlag::Z, cpu.a == data);
    cpu.SetFlag(StatusFlag::N, result & 0x80);
}

DEFINE_OPERATION(CPX)
{
    const u8 data = cpu.ReadByte(address);
    const u8 result = cpu.x - data;

    cpu.SetFlag(StatusFlag::C, cpu.x >= data);
    cpu.SetFlag(StatusFlag::Z, cpu.x == data);
    cpu.SetFlag(StatusFlag::N, result & 0x80);
}

DEFINE_OPERATION(CPY)
{
    const u8 data = cpu.ReadByte(address);
    const u8 result = cpu.y - data;

    cpu.SetFlag(StatusFlag::C, cpu.y >= data);
    cpu.SetFlag(StatusFlag::Z, cpu.y == data);
    cpu.SetFlag(StatusFlag::N, result & 0x80);
}

DEFINE_OPERATION(DEC)
{
    u8 data = cpu.ReadByte(address);
    data = (data - 1) & 0x00ff;
    cpu.WriteByte(address, data);

    cpu.SetFlag(StatusFlag::Z, data == 0);
    cpu.SetFlag(StatusFlag::N, data & 0x80);
}

DEFINE_OPERATION(DEX)
{
    cpu.x = (cpu.x - 1) & 0x00ff;

    cpu.SetFlag(StatusFlag::Z, cpu.x == 0);
    cpu.SetFlag(StatusFlag::N, cpu.x & 0x80);
}

DEFINE_OPERATION(DEY)
{
    cpu.y = (cpu.y - 1) & 0x00ff;

    cpu.SetFlag(StatusFlag::Z, cpu.y == 0);
    cpu.SetFlag(StatusFlag::N, cpu.y & 0x80);
}

DEFINE_OPERATION(EOR)
{
    u8 data = cpu.ReadByte(address);

    cpu.a ^= data;

    cpu.SetFlag(StatusFlag::Z, cpu.a == 0);
    cpu.SetFlag(StatusFlag::N, cpu.a & 0x80);
}

DEFINE_OPERATION(INC)
{
    u8 data = cpu.ReadByte(address);
    data = (data + 1) & 0x00ff;
    cpu.WriteByte(address, data);

    cpu.SetFlag(StatusFlag::Z, data == 0);
    cpu.SetFlag(StatusFlag::N, data & 0x80);
}

DEFINE_OPERATION(INX)
{
    cpu.x = (cpu.x + 1) & 0x00ff;

    cpu.SetFlag(StatusFlag::Z, cpu.x == 0);
    cpu.SetFlag(StatusFlag::N, cpu.x & 0x80);
}

DEFINE_OPERATION(INY)
{
    cpu.y = (cpu.y + 1) & 0x00ff;

    cpu.SetFlag(StatusFlag::Z, cpu.y == 0);
    cpu.SetFlag(StatusFlag::N, cpu.y & 0x80);
}

DEFINE_OPERATION(JMP)
{
    cpu.pc = address;
}

DEFINE_OPERATION(JSR)
{
    cpu.PushToStack((cpu.pc - 1) >> 8);
    cpu.PushToStack((cpu.pc - 1) & 0x00ff);
    cpu.pc = address;
}

DEFINE_OPERATION(LDA)
{
    cpu.a = cpu.ReadByte(address);

    cpu.SetFlag(StatusFlag::Z, cpu.a == 0);
    cpu.SetFlag(StatusFlag::N, cpu.a & 0x80);
}

DEFINE_OPERATION(LDX)
{
    cpu.x = cpu.ReadByte(address);

    cpu.SetFlag(StatusFlag::Z, cpu.x == 0);
    cpu.SetFlag(StatusFlag::N, cpu.x & 0x80);
}

DEFINE_OPERATION(LDY)
{
    cpu.y = cpu.ReadByte(address);

    cpu.SetFlag(StatusFlag::Z, cpu.y == 0);
    cpu.SetFlag(StatusFlag::N, cpu.y & 0x80);
}

DEFINE_OPERATION_NO_ADDRESS(LSR)
{
    cpu.SetFlag(StatusFlag::C, cpu.a & 0x01);

    cpu.a >>= 1;

    cpu.SetFlag(StatusFlag::Z, cpu.a == 0);
    cpu.SetFlag(StatusFlag::N, cpu.a & 0x80);
}

DEFINE_OPERATION(LSR)
{
    u8 data = cpu.ReadByte(address);
    
    cpu.SetFlag(StatusFlag::C, data & 0x01);
    data >>= 1;

    cpu.WriteByte(address, data);

    cpu.SetFlag(StatusFlag::Z, data == 0);
    cpu.SetFlag(StatusFlag::N, data & 0x80);
}

DEFINE_OPERATION(ORA)
{
    u8 data = cpu.ReadByte(address);

    cpu.a |= data;

    cpu.SetFlag(StatusFlag::Z, cpu.a == 0);
    cpu.SetFlag(StatusFlag::N, cpu.a & 0x80);
}

DEFINE_OPERATION_NO_ADDRESS(NOP)
{
    // Silence is golden.
}

DEFINE_OPERATION_NO_ADDRESS(PHA)
{
    cpu.PushToStack(cpu.a);
}

DEFINE_OPERATION_NO_ADDRESS(PHP)
{
    cpu.PushToStack(cpu.s | StatusFlag::B | StatusFlag::U);
}

DEFINE_OPERATION_NO_ADDRESS(PLA)
{
    cpu.a = cpu.PullFromStack();
    cpu.SetFlag(StatusFlag::Z, cpu.a == 0);
    cpu.SetFlag(StatusFlag::N, cpu.a & 0x80);
}

// TODO: the I flag should be set 1 instruction later
// Check if it can cause issues
DEFINE_OPERATION_NO_ADDRESS(PLP)
{
    cpu.s = cpu.PullFromStack();
}

DEFINE_OPERATION_NO_ADDRESS(ROL)
{
    const bool carry = cpu.a >> 7;
    cpu.a = ((cpu.a << 1) | cpu.GetFlag(StatusFlag::C)) & 0xff;

    cpu.SetFlag(StatusFlag::C, carry);
    cpu.SetFlag(StatusFlag::Z, cpu.a == 0);
    cpu.SetFlag(StatusFlag::N, cpu.a & 0x80);
}

DEFINE_OPERATION(ROL)
{
    u8 data = cpu.ReadByte(address);
    const bool carry = data >> 7;
    data = ((data << 1) | cpu.GetFlag(StatusFlag::C)) & 0xff;

    cpu.WriteByte(address, data);

    cpu.SetFlag(StatusFlag::C, carry);
    cpu.SetFlag(StatusFlag::Z, data == 0);
    cpu.SetFlag(StatusFlag::N, data & 0x80);
}

DEFINE_OPERATION_NO_ADDRESS(ROR)
{
    const bool carry = cpu.a & 0x01;
    cpu.a = (cpu.a >> 1) | (cpu.GetFlag(StatusFlag::C) << 7);

    cpu.SetFlag(StatusFlag::C, carry);
    cpu.SetFlag(StatusFlag::Z, cpu.a == 0);
    cpu.SetFlag(StatusFlag::N, cpu.a & 0x80);
}

DEFINE_OPERATION(ROR)
{
    u8 data = cpu.ReadByte(address);
    const bool carry = data & 0x01;
    data = (data >> 1) | (cpu.GetFlag(StatusFlag::C) << 7);
    
    cpu.WriteByte(address, data);

    cpu.SetFlag(StatusFlag::C, carry);
    cpu.SetFlag(StatusFlag::Z, data == 0);
    cpu.SetFlag(StatusFlag::N, data & 0x80);
}

DEFINE_OPERATION(RTI)
{
    cpu.s = cpu.PullFromStack();
    cpu.s &= ~StatusFlag::B;
    cpu.s |= StatusFlag::U;

    const u8 pc_low = cpu.PullFromStack();
    const u8 pc_high = cpu.PullFromStack();
    cpu.pc = static_cast<u16>((pc_high << 8) | pc_low);
}

DEFINE_OPERATION(RTS)
{
    const u8 pc_low = cpu.PullFromStack();
    const u8 pc_high = cpu.PullFromStack();

    cpu.pc = static_cast<u16>((pc_high << 8) | pc_low) + 1;
}

DEFINE_OPERATION(SBC)
{
    u8 data = cpu.ReadByte(address) ^ 0x00ff;
    u16 temp = cpu.a + data + cpu.GetFlag(StatusFlag::C);

    cpu.SetFlag(StatusFlag::C, temp & 0xff00);
    cpu.SetFlag(StatusFlag::Z, (temp & 0x00ff) == 0);
    cpu.SetFlag(StatusFlag::V, (temp ^ cpu.a) & (temp ^ data) & 0x0080);
    cpu.SetFlag(StatusFlag::N, temp & 0x0080);

    cpu.a = temp & 0x00ff;
}

DEFINE_OPERATION_NO_ADDRESS(SEC)
{
    cpu.SetFlag(StatusFlag::C, true);
}

DEFINE_OPERATION_NO_ADDRESS(SED)
{
    cpu.SetFlag(StatusFlag::D, true);
}

// TODO: the I flag should be set 1 instruction later
// Check if it can cause issues
DEFINE_OPERATION_NO_ADDRESS(SEI)
{
    cpu.SetFlag(StatusFlag::I, true);
}

DEFINE_OPERATION(STA)
{
    cpu.WriteByte(address, cpu.a);
}

DEFINE_OPERATION(STX)
{
    cpu.WriteByte(address, cpu.x);
}

DEFINE_OPERATION(STY)
{
    cpu.WriteByte(address, cpu.y);
}

DEFINE_OPERATION(TAX)
{
    cpu.x = cpu.a;
    cpu.SetFlag(StatusFlag::Z, cpu.x == 0);
    cpu.SetFlag(StatusFlag::N, cpu.x & 0x80);
}

DEFINE_OPERATION(TAY)
{
    cpu.y = cpu.a;
    cpu.SetFlag(StatusFlag::Z, cpu.y == 0);
    cpu.SetFlag(StatusFlag::N, cpu.y & 0x80);
}

DEFINE_OPERATION_NO_ADDRESS(TSX)
{
    cpu.x = cpu.sp;
    cpu.SetFlag(StatusFlag::Z, cpu.x == 0);
    cpu.SetFlag(StatusFlag::N, cpu.x & 0x80);
}

DEFINE_OPERATION(TXA)
{
    cpu.a = cpu.x;
    cpu.SetFlag(StatusFlag::Z, cpu.a == 0);
    cpu.SetFlag(StatusFlag::N, cpu.a & 0x80);
}

DEFINE_OPERATION_NO_ADDRESS(TXS)
{
    cpu.sp = cpu.x;
}

DEFINE_OPERATION(TYA)
{
    cpu.a = cpu.y;
    cpu.SetFlag(StatusFlag::Z, cpu.a == 0);
    cpu.SetFlag(StatusFlag::N, cpu.a & 0x80);
}

#pragma endregion
