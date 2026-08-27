#include <gtest/gtest.h>

#include "test-utils/utils.hpp"
#include "test-utils/instruction-fixture.hpp"

TEST(INX, BasicXIncrement)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->cpu->x = 0x42;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::INX_IMP,
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->x, 0x43);
}

TEST(INX, SetZeroFlag)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->cpu->x = 0xff;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::INX_IMP
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->x, 0x00);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
}

TEST(INX, SetNegativeFlag)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->cpu->x = 0x7f;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::INX_IMP
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->x, 0x80);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
}

INSTANTIATE_INSTRUCTION_TEST(
    INX,
    OpcodeTest { Opcodes::INX_IMP, TestAddressingMode::ZeroPage, 2 }
);
