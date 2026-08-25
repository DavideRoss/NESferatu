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

    cpu->pc = 0x0000;
    cpu->x = 0xff;
    mem->WriteSeq(0x0000, {
        Opcodes::LDA_ABX, 0x66, 0x05
    });
    mem->Write(0x056b, 0x8c);

    u32 cycles = cpu->Step();
    (void)cycles;

    return 0;
}
