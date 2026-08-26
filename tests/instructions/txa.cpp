#include <gtest/gtest.h>

#include "test-utils/utils.hpp"

TEST(TXA, TransferXToA)
{
    System* nes = TestUtils::SetupSystem();
    
    nes->cpu->pc = 0x000;
    nes->cpu->x = 0x10;

    nes->mem->Write(0x0000, Opcodes::TXA_IMP);

    u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(nes->cpu->a, 0x10);
}

TEST(TXA, SetZeroFlag)
{
    System* nes = TestUtils::SetupSystem();

    nes->cpu->pc = 0x000;
    nes->cpu->x = 0x00;

    nes->mem->Write(0x0000, Opcodes::TXA_IMP);

    nes->cpu->Step();
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
}

TEST(TXA, SetNegativeFlag)
{
    System* nes = TestUtils::SetupSystem();

    nes->cpu->pc = 0x000;
    nes->cpu->x = 0x8a;

    nes->mem->Write(0x0000, Opcodes::TXA_IMP);

    nes->cpu->Step();
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
}
