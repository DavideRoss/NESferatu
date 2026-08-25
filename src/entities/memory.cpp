#include "memory.h"

#include <assert.h>

Memory::Memory()
{
    for (u32 i = 0; i < 2048; i++)
    {
        memory_[i] = 0x0;
    }
}

void Memory::ConnectBus(Bus *bus)
{
    bus_ = bus;
}

u8 Memory::Read(u16 address) const
{
    assert(address < 0x0800);
    return memory_[address];
}

void Memory::Write(u16 address, u8 value)
{
    assert(address < 0x800);
    memory_[address] = value;
}

void Memory::WriteSeq(u16 address, std::vector<u8> data)
{
    for (u8 val : data)
    {
        Write(address++, val);
    }
}
