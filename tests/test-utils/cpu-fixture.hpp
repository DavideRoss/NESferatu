#include <gtest/gtest.h>

#include "entities/system.h"

class CPUTest : public ::testing::Test
{
protected:
    System* nes;

    void SetUp() override
    {
        CPU* cpu = new CPU();
        Memory* mem = new Memory();
        Bus* bus = new Bus(cpu, mem);

        cpu->ConnectBus(bus);
        mem->ConnectBus(bus);

        nes = new System(bus, cpu, mem);
        nes->Reset();
        nes->cpu->pc = 0x0000;
    }

    void Load(u16 start_address, std::initializer_list<u8> data)
    {
        nes->mem->WriteSeq(start_address, data);
    }

    u32 Step()
    {
        return nes->cpu->Step();
    }
};
