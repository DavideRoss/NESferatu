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

void CPU::Execute(const u32 cycles)
{
    cycles_ = cycles;
    while (cycles_ > 0)
    {
        [[maybe_unused]] u8 instruction = FetchByte();

        switch (instruction)
        {
            case Opcodes::LDA_IM: LDAImmediate(); break;
            case Opcodes::LDA_ZP: LDAZeroPage(); break;
            case Opcodes::LDA_ZPX: LDAZeroPageX(); break;

            default: throw std::runtime_error("Unimplemented instruction");
        }
    }
}

u8 CPU::FetchByte()
{
    u8 data = bus_->ReadByte(pc);
    pc++;
    cycles_--;
    return data;
}

u8 CPU::PeekByte(u16 address)
{
    u8 data = bus_->ReadByte(address);
    cycles_--;
    return data;
}

void CPU::SetFlag(StatusFlag flag, bool val)
{
    s = val ? s | flag : s & ~flag;
}

void CPU::LDAImmediate()
{
    u8 data = FetchByte();
    a = data;

    SetFlag(StatusFlag::Z, a == 0);
    SetFlag(StatusFlag::N, (a & 0x80) > 0);
}

void CPU::LDAZeroPage()
{
    u8 zero_page = FetchByte();
    a = PeekByte(zero_page);

    SetFlag(StatusFlag::Z, a == 0);
    SetFlag(StatusFlag::N, (a & 0x80) > 0);
}

void CPU::LDAZeroPageX()
{
    u8 zero_page = FetchByte();
    zero_page = (zero_page + x) & 0xff;
    cycles_--;
    a = PeekByte(zero_page);

    SetFlag(StatusFlag::Z, a == 0);
    SetFlag(StatusFlag::N, (a & 0x80) > 0);
}
