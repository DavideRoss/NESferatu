#include "cpu.h"

#include <stdexcept>

#include "bus.h"

#define EXECUTE_OPCODE(opcode, operation, addressing_mode, page_cross, extra_cycles) \
    case Opcodes::opcode: Execute<operation, addressing_mode, page_cross, extra_cycles>(); break;

void CPU::ConnectBus(Bus *bus)
{
    bus_ = bus;
}

void CPU::Reset()
{
    a = x = y = 0x0;
    pc = 0xfffc;
    sp = 0xfd;
    s = 0x00;
}

u32 CPU::Step()
{
    cycles_ = 0;
    
    u8 instruction = FetchByte();
    switch (instruction)
    {
        // Add with Carry
        EXECUTE_OPCODE(ADC_IM,  ADC, Immediate,  false, 0)
        EXECUTE_OPCODE(ADC_ZP,  ADC, ZeroPage,   false, 0)
        EXECUTE_OPCODE(ADC_ZPX, ADC, ZeroPageX,  false, 0)
        EXECUTE_OPCODE(ADC_ABS, ADC, Absolute,   false, 0)
        EXECUTE_OPCODE(ADC_ABX, ADC, AbsoluteX,  true,  0)
        EXECUTE_OPCODE(ADC_ABY, ADC, AbsoluteY,  true,  0)
        EXECUTE_OPCODE(ADC_IDX, ADC, IndirectX,  false, 0)
        EXECUTE_OPCODE(ADC_IDY, ADC, IndirectY,  true,  0)

        // Logical AND
        EXECUTE_OPCODE(AND_IM,  AND, Immediate,  false, 0)
        EXECUTE_OPCODE(AND_ZP,  AND, ZeroPage,   false, 0)
        EXECUTE_OPCODE(AND_ZPX, AND, ZeroPageX,  false, 0)
        EXECUTE_OPCODE(AND_ABS, AND, Absolute,   false, 0)
        EXECUTE_OPCODE(AND_ABX, AND, AbsoluteX,  true,  0)
        EXECUTE_OPCODE(AND_ABY, AND, AbsoluteY,  true,  0)
        EXECUTE_OPCODE(AND_IDX, AND, IndirectX,  false, 0)
        EXECUTE_OPCODE(AND_IDY, AND, IndirectY,  true,  0)

        // Load Accumulator
        EXECUTE_OPCODE(LDA_IM,  LDA, Immediate,  false, 0)
        EXECUTE_OPCODE(LDA_ZP,  LDA, ZeroPage,   false, 0)
        EXECUTE_OPCODE(LDA_ZPX, LDA, ZeroPageX,  false, 0)
        EXECUTE_OPCODE(LDA_ABS, LDA, Absolute,   false, 0)
        EXECUTE_OPCODE(LDA_ABX, LDA, AbsoluteX,  true,  0)
        EXECUTE_OPCODE(LDA_ABY, LDA, AbsoluteY,  true,  0)
        EXECUTE_OPCODE(LDA_IDX, LDA, IndirectX,  false, 0)
        EXECUTE_OPCODE(LDA_IDY, LDA, IndirectY,  true,  0)

        // Load X Register
        EXECUTE_OPCODE(LDX_IM,  LDX, Immediate,  false, 0)
        EXECUTE_OPCODE(LDX_ZP,  LDX, ZeroPage,   false, 0)
        EXECUTE_OPCODE(LDX_ZPY, LDX, ZeroPageY,  false, 0)
        EXECUTE_OPCODE(LDX_ABS, LDX, Absolute,   false, 0)
        EXECUTE_OPCODE(LDX_ABY, LDX, AbsoluteY,  true,  0)

        // Load Y Register
        EXECUTE_OPCODE(LDY_IM,  LDY, Immediate,  false, 0)
        EXECUTE_OPCODE(LDY_ZP,  LDY, ZeroPage,   false, 0)
        EXECUTE_OPCODE(LDY_ZPX, LDY, ZeroPageX,  false, 0)
        EXECUTE_OPCODE(LDY_ABS, LDY, Absolute,   false, 0)
        EXECUTE_OPCODE(LDY_ABX, LDY, AbsoluteX,  true,  0)

        // Store Accumulator
        EXECUTE_OPCODE(STA_ZP,  STA, ZeroPage,   false, 0)
        EXECUTE_OPCODE(STA_ZPX, STA, ZeroPageX,  false, 0)
        EXECUTE_OPCODE(STA_ABS, STA, Absolute,   false, 0)
        EXECUTE_OPCODE(STA_ABX, STA, AbsoluteX,  false, 1)
        EXECUTE_OPCODE(STA_ABY, STA, AbsoluteY,  false, 1)
        EXECUTE_OPCODE(STA_IDX, STA, IndirectX,  false, 0)
        EXECUTE_OPCODE(STA_IDY, STA, IndirectY,  false, 1)

        // Store X Register
        EXECUTE_OPCODE(STX_ZP,  STX, ZeroPage,   false, 0)
        EXECUTE_OPCODE(STX_ZPY, STX, ZeroPageY,  false, 0)
        EXECUTE_OPCODE(STX_ABS, STX, Absolute,   false, 0)

        // Store Y Register
        EXECUTE_OPCODE(STY_ZP,  STY, ZeroPage,   false, 0)
        EXECUTE_OPCODE(STY_ZPX, STY, ZeroPageX,  false, 0)
        EXECUTE_OPCODE(STY_ABS, STY, Absolute,   false, 0)

        // Transfer Accumulator to X
        EXECUTE_OPCODE(TAX_IMP, TAX, Implied,    false, 1)

        default: throw std::runtime_error("Unimplemented instruction");
    }

    return cycles_;
}

u8 CPU::FetchByte()
{
    const u8 data = bus_->ReadByte(pc);
    pc++;
    cycles_++;
    return data;
}

u8 CPU::ReadByte(u16 address)
{
    const u8 data = bus_->ReadByte(address);
    cycles_++;
    return data;
}

void CPU::WriteByte(u16 address, u8 data)
{
    bus_->WriteByte(address, data);
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

#pragma region Addressing Modes

DEFINE_ADDRESSING_MODE(Implied)
{
    return {};
}

DEFINE_ADDRESSING_MODE(Immediate)
{
    return
    {
        cpu.pc++,
        false
    };
}

DEFINE_ADDRESSING_MODE(ZeroPage)
{
    return
    {
        cpu.FetchByte(),
        false
    };
}

DEFINE_ADDRESSING_MODE(ZeroPageX)
{
    u8 zero_page = cpu.FetchByte();
    zero_page = (zero_page + cpu.x) & 0xff;
    cycles++;

    return
    {
        zero_page,
        false
    };
}

DEFINE_ADDRESSING_MODE(ZeroPageY)
{
    u8 zero_page = cpu.FetchByte();
    zero_page = (zero_page + cpu.y) & 0xff;
    cycles++;

    return
    {
        zero_page,
        false
    };
}

DEFINE_ADDRESSING_MODE(Absolute)
{
    const u8 addr_low = cpu.FetchByte();
    const u8 addr_high = cpu.FetchByte();
    return
    {
        static_cast<u16>((addr_high << 8) | addr_low),
        false
    };
}

DEFINE_ADDRESSING_MODE(AbsoluteX)
{
    const u8 addr_low = cpu.FetchByte();
    const u8 addr_high = cpu.FetchByte();
    u16 addr = static_cast<u16>((addr_high << 8) | addr_low);

    addr += cpu.x;

    return
    {
        addr,
        (addr & 0xff00) != (addr_high << 8)
    };
}

DEFINE_ADDRESSING_MODE(AbsoluteY)
{
    const u8 addr_low = cpu.FetchByte();
    const u8 addr_high = cpu.FetchByte();
    u16 addr = static_cast<u16>((addr_high << 8) | addr_low);

    addr += cpu.y;

    return
    {
        addr,
        (addr & 0xff00) != (addr_high << 8)
    };
}

DEFINE_ADDRESSING_MODE(IndirectX)
{
    const u16 zero_page = cpu.FetchByte();
    const u16 addr_low = cpu.ReadByte((zero_page + cpu.x) & 0x00ff);
    const u16 addr_high = cpu.ReadByte((zero_page + cpu.x + 1) & 0x00ff);
    cycles++;

    return
    {
        static_cast<u16>((addr_high << 8) | addr_low),
        false
    };
}

DEFINE_ADDRESSING_MODE(IndirectY)
{
    const u16 zero_page = cpu.FetchByte();
    const u16 addr_low = cpu.ReadByte(zero_page & 0x00ff);
    const u16 addr_high = cpu.ReadByte((zero_page + 1) & 0x00ff);

    u16 addr = (addr_high << 8) | addr_low;
    addr += cpu.y;

    return
    {
        addr,
        (addr & 0xff00) != (addr_high << 8)
    };
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
    cpu.x = cpu.a;cpu.SetFlag(StatusFlag::Z, cpu.x == 0);
    cpu.SetFlag(StatusFlag::N, cpu.x & 0x80);
}

#pragma endregion
