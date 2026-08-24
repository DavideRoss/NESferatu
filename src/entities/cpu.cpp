#include "cpu.h"

#include "bus.h"

void CPU::ConnectBus(Bus *bus)
{
    bus_ = bus;
}

void CPU::Reset()
{
    a = x = y = 0x0;
    pc = 0xfffc;
    s = 0x0;
    p = 0x36;
}
