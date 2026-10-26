#pragma once

#include <initializer_list>
#include <array>

#include "utils/types.h"

class Bus;

class Memory
{
public:
    Memory();

    void Reset();

    void ConnectBus(Bus* bus);

    u8 Read(u16 address) const;
    void Write(u16 address, u8 value);
    void WriteSeq(u16 address, std::initializer_list<u8> data);

    void Serialize(class Serializer& serializer);
    void Deserialize(class Deserializer& deserializer);

private:
    Bus* bus_ = nullptr;

    std::array<u8, 2048> memory_;
};