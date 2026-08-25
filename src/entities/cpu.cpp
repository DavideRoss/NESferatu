#include "cpu.h"

#include <stdexcept>

#include "bus.h"

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
        case Opcodes::ADC_IM: Execute<ADC, Immediate>(); break;
        case Opcodes::ADC_ZP: Execute<ADC, ZeroPage>(); break;
        case Opcodes::ADC_ZPX: Execute<ADC, ZeroPageX>(); break;
        case Opcodes::ADC_ABS: Execute<ADC, Absolute>(); break;
        case Opcodes::ADC_ABX: Execute<ADC, AbsoluteX>(); break;
        case Opcodes::ADC_ABY: Execute<ADC, AbsoluteY>(); break;
        case Opcodes::ADC_IDX: Execute<ADC, IndirectX>(); break;
        case Opcodes::ADC_IDY: Execute<ADC, IndirectY>(); break;

        // Logical AND
        case Opcodes::AND_IM: Execute<AND, Immediate>(); break;
        case Opcodes::AND_ZP: Execute<AND, ZeroPage>(); break;
        case Opcodes::AND_ZPX: Execute<AND, ZeroPageX>(); break;
        case Opcodes::AND_ABS: Execute<AND, Absolute>(); break;
        case Opcodes::AND_ABX: Execute<AND, AbsoluteX>(); break;
        case Opcodes::AND_ABY: Execute<AND, AbsoluteY>(); break;
        case Opcodes::AND_IDX: Execute<AND, IndirectX>(); break;
        case Opcodes::AND_IDY: Execute<AND, IndirectY>(); break;

        // Load Accumulator
        case Opcodes::LDA_IM: Execute<LDA, Immediate>(); break;
        case Opcodes::LDA_ZP: Execute<LDA, ZeroPage>(); break;
        case Opcodes::LDA_ZPX: Execute<LDA, ZeroPageX>(); break;
        case Opcodes::LDA_ABS: Execute<LDA, Absolute>(); break;
        case Opcodes::LDA_ABX: Execute<LDA, AbsoluteX>(); break;
        case Opcodes::LDA_ABY: Execute<LDA, AbsoluteY>(); break;
        case Opcodes::LDA_IDX: Execute<LDA, IndirectX>(); break;
        case Opcodes::LDA_IDY: Execute<LDA, IndirectY>(); break;

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

u8 CPU::GetFlag(StatusFlag flag)
{
    return ((s & flag) > 0) ? 0x1 : 0x0;
}

void CPU::SetFlag(StatusFlag flag, bool val)
{
    s = val ? s | flag : s & ~flag;
}

#pragma region Addressing Modes

DEFINE_ADDRESSING_MODE(Immediate)
{
    return cpu.pc++;
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

DEFINE_ADDRESSING_MODE(Absolute)
{
    const u8 addr_low = cpu.FetchByte();
    const u8 addr_high = cpu.FetchByte();
    return (addr_high << 8) | addr_low;
}

DEFINE_ADDRESSING_MODE(AbsoluteX)
{
    const u8 addr_low = cpu.FetchByte();
    const u8 addr_high = cpu.FetchByte();
    u16 addr = (addr_high << 8) | addr_low;

    addr += cpu.x;

    if ((addr & 0xff00) != (addr_high << 8))
    {
        cycles++;
    }

    return addr;
}

DEFINE_ADDRESSING_MODE(AbsoluteY)
{
    const u8 addr_low = cpu.FetchByte();
    const u8 addr_high = cpu.FetchByte();
    u16 addr = (addr_high << 8) | addr_low;

    addr += cpu.y;

    if ((addr & 0xff00) != (addr_high << 8))
    {
        cycles++;
    }

    return addr;
}

DEFINE_ADDRESSING_MODE(IndirectX)
{
    const u16 zero_page = cpu.FetchByte();
    const u16 addr_low = cpu.ReadByte((zero_page + cpu.x) & 0x00ff);
    const u16 addr_high = cpu.ReadByte((zero_page + cpu.x + 1) & 0x00ff);
    cycles++;

    return (addr_high << 8) | addr_low;
}

DEFINE_ADDRESSING_MODE(IndirectY)
{
    const u16 zero_page = cpu.FetchByte();
    const u16 addr_low = cpu.ReadByte(zero_page & 0x00ff);
    const u16 addr_high = cpu.ReadByte((zero_page + 1) & 0x00ff);

    u16 addr = (addr_high << 8) | addr_low;
    addr += cpu.y;

    if ((addr & 0xff00) != (addr_high << 8))
    {
        cycles++;
    }

    return addr;
}

#pragma endregion

#pragma region Operations

DEFINE_OPERATION(LDA)
{
    cpu.a = cpu.ReadByte(address);

    cpu.SetFlag(StatusFlag::Z, cpu.a == 0);
    cpu.SetFlag(StatusFlag::N, cpu.a & 0x80);
}

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

#pragma endregion
