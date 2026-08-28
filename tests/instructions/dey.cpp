#include <gtest/gtest.h>

#include "test-utils/instruction-fixture.hpp"

TEST(DEY, BasicYDecrement)
{
    System* nes = new System();
    nes->cpu->pc = 0x000;

    nes->cpu->y = 0x42;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::DEY_IMP,
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->y, 0x41);
}

TEST(DEY, SetZeroFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x000;

    nes->cpu->y = 0x01;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::DEY_IMP
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->y, 0x00);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
}

TEST(DEY, SetNegativeFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x000;

    nes->cpu->y = 0x00;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::DEY_IMP
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->y, 0xff);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
}

INSTANTIATE_INSTRUCTION_TEST(
    DEY,
    OpcodeTest { Opcodes::DEY_IMP, TestAddressingMode::ZeroPage, 2 }
);
