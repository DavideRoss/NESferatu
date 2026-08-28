#include <gtest/gtest.h>

#include "entities/system.h"

TEST(TAX, TransferAToX)
{
    System* nes = new System();
    
    nes->cpu->pc = 0x000;
    nes->cpu->a = 0x10;

    nes->mem->Write(0x0000, Opcodes::TAX_IMP);

    u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(nes->cpu->x, 0x10);
}

TEST(TAX, SetZeroFlag)
{
    System* nes = new System();

    nes->cpu->pc = 0x000;
    nes->cpu->a = 0x00;

    nes->mem->Write(0x0000, Opcodes::TAX_IMP);

    nes->cpu->Step();
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
}

TEST(TAX, SetNegativeFlag)
{
    System* nes = new System();

    nes->cpu->pc = 0x000;
    nes->cpu->a = 0x8a;

    nes->mem->Write(0x0000, Opcodes::TAX_IMP);

    nes->cpu->Step();
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
}
