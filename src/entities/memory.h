#pragma once

#include "utils/types.h"

class Bus;

class Memory
{
public:
    Memory();

    void ConnectBus(Bus* bus);

    u8 Read(u16 address) const;
    void Write(u16 address, u8 value);

private:
    Bus* bus_ = nullptr;

    u8 memory_[2048];
};
