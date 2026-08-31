#pragma once

#include <initializer_list>

#include "utils/types.h"

class Bus;

class Memory
{
public:
    Memory();

    void ConnectBus(Bus* bus);

    u8 Read(u16 address) const;
    void Write(u16 address, u8 value);
    void WriteSeq(u16 address, std::initializer_list<u8> data);

private:
    Bus* bus_ = nullptr;

    u8 memory_[2048];
};
