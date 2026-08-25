#include <iostream>

#include "entities/system.h"

int main()
{
    std::cout << "Hello, NESferatu!\n";

    CPU* cpu = new CPU();
    Memory* mem = new Memory();
    Bus* bus = new Bus(cpu, mem);

    // TODO: move everything inside system?
    cpu->ConnectBus(bus);
    mem->ConnectBus(bus);

    System* nes = new System(bus, cpu, mem);
    nes->Reset();

    return 0;
}
