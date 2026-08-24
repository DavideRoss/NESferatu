#include "memory.h"

#include "bus.h"

void Memory::ConnectBus(Bus *bus)
{
    bus_ = bus;
}
