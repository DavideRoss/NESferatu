#include "apu.h"

void APU::ConnectBus(Bus* bus)
{
    bus_ = bus;
}

void APU::Reset()
{
}

u8 APU::CPURead(u16 address)
{
    // TODO: fill stub
    return 0x00;
}

void APU::CPUWrite(u16 address, u8 val)
{
    // TODO: fill stub
}
