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
        case Opcodes::LDA_IM: Execute<LDA, Immediate>(); break;
        case Opcodes::LDA_ZP: Execute<LDA, ZeroPage>(); break;
        case Opcodes::LDA_ZPX: Execute<LDA, ZeroPageX>(); break;
        case Opcodes::LDA_ABS: Execute<LDA, Absolute>(); break;
        case Opcodes::LDA_ABX: Execute<LDA, AbsoluteX>(); break;
        case Opcodes::LDA_ABY: Execute<LDA, AbsoluteY>(); break;

        default: throw std::runtime_error("Unimplemented instruction");
    }

    return cycles_;
}

u8 CPU::FetchByte()
{
    u8 data = bus_->ReadByte(pc);
    pc++;
    cycles_++;
    return data;
}

u8 CPU::ReadByte(u16 address)
{
    u8 data = bus_->ReadByte(address);
    cycles_++;
    return data;
}

void CPU::SetFlag(StatusFlag flag, bool val)
{
    s = val ? s | flag : s & ~flag;
}

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
    u8 addr_low = cpu.FetchByte();
    u8 addr_high = cpu.FetchByte();
    return (addr_high << 8) | addr_low;
}

DEFINE_ADDRESSING_MODE(AbsoluteX)
{
    u8 addr_low = cpu.FetchByte();
    u8 addr_high = cpu.FetchByte();
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
    u8 addr_low = cpu.FetchByte();
    u8 addr_high = cpu.FetchByte();
    u16 addr = (addr_high << 8) | addr_low;

    addr += cpu.y;

    if ((addr & 0xff00) != (addr_high << 8))
    {
        cycles++;
    }

    return addr;
}

DEFINE_OPERATION(LDA)
{
    cpu.a = cpu.ReadByte(address);

    cpu.SetFlag(StatusFlag::Z, cpu.a == 0);
    cpu.SetFlag(StatusFlag::N, (cpu.a & 0x80) > 0);
}
