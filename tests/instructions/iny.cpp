#include <gtest/gtest.h>

#include "test-utils/utils.hpp"
#include "test-utils/instruction-fixture.hpp"

TEST(INY, BasicYIncrement)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;

    nes->cpu->y = 0x42;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::INY_IMP,
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->y, 0x43);
}

TEST(INY, SetZeroFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;

    nes->cpu->y = 0xff;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::INY_IMP
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->y, 0x00);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
}

TEST(INY, SetNegativeFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;

    nes->cpu->y = 0x7f;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::INY_IMP
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->y, 0x80);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
}

INSTANTIATE_INSTRUCTION_TEST(
    INY,
    OpcodeTest { Opcodes::INY_IMP, TestAddressingMode::ZeroPage, 2 }
);
