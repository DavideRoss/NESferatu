#include <gtest/gtest.h>

#include "test-utils/utils.hpp"

#include "entities/system.h"

TEST(TYA, TransferYToA)
{
    System* nes = TestUtils::CreateTestSystem();
    
    nes->cpu->pc = 0x0000;
    nes->cpu->y = 0x10;

    nes->mem->Write(0x0000, Opcodes::TYA_IMP);

    u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(nes->cpu->a, 0x10);
}

TEST(TYA, SetZeroFlag)
{
    System* nes = TestUtils::CreateTestSystem();

    nes->cpu->pc = 0x0000;
    nes->cpu->y = 0x00;

    nes->mem->Write(0x0000, Opcodes::TYA_IMP);

    nes->cpu->Step();
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
}

TEST(TYA, SetNegativeFlag)
{
    System* nes = TestUtils::CreateTestSystem();

    nes->cpu->pc = 0x0000;
    nes->cpu->y = 0x8a;

    nes->mem->Write(0x0000, Opcodes::TYA_IMP);

    nes->cpu->Step();
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
}
