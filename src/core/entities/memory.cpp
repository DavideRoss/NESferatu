#include "memory.h"

#include <assert.h>
#include <algorithm>

#include "savestates/serializer.h"
#include "savestates/deserializer.h"

Memory::Memory()
{
    Reset();
}

void Memory::Reset()
{
    std::fill(memory_.begin(), memory_.end(), 0x00);
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

void Memory::WriteSeq(u16 address, std::initializer_list<u8> data)
{
    for (u8 val : data)
    {
        Write(address++, val);
    }
}

void Memory::Serialize(Serializer& serializer)
{
    serializer.Write(memory_);
}

void Memory::Deserialize(Deserializer &deserializer)
{
    deserializer.Read(memory_);
}
