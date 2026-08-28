#include <gtest/gtest.h>

#include "test-utils/instruction-fixture.hpp"

TEST(DEX, BasicXDecrement)
{
    System* nes = new System();
    nes->cpu->pc = 0x000;

    nes->cpu->x = 0x42;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::DEX_IMP,
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->x, 0x41);
}

TEST(DEX, SetZeroFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x000;

    nes->cpu->x = 0x01;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::DEX_IMP
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->x, 0x00);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
}

TEST(DEX, SetNegativeFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x000;

    nes->cpu->x = 0x00;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::DEX_IMP
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->x, 0xff);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
}

INSTANTIATE_INSTRUCTION_TEST(
    DEX,
    OpcodeTest { Opcodes::DEX_IMP, TestAddressingMode::ZeroPage, 2 }
);
