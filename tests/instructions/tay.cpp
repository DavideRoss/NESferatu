#include <gtest/gtest.h>

#include "entities/system.h"

TEST(TAY, TransferAToY)
{
    System* nes = new System();
    
    nes->cpu->pc = 0x0000;
    nes->cpu->a = 0x10;

    nes->mem->Write(0x0000, Opcodes::TAY_IMP);

    u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(nes->cpu->y, 0x10);
}

TEST(TAY, SetZeroFlag)
{
    System* nes = new System();

    nes->cpu->pc = 0x0000;
    nes->cpu->a = 0x00;

    nes->mem->Write(0x0000, Opcodes::TAY_IMP);

    nes->cpu->Step();
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
}

TEST(TAY, SetNegativeFlag)
{
    System* nes = new System();

    nes->cpu->pc = 0x0000;
    nes->cpu->a = 0x8a;

    nes->mem->Write(0x0000, Opcodes::TAY_IMP);

    nes->cpu->Step();
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
}
