#include <gtest/gtest.h>

#include "test-utils/utils.hpp"
#include "test-utils/instruction-fixture.hpp"

TEST(LDY, SetZeroFlag)
{
    System* nes = TestUtils::SetupSystem();

    nes->cpu->pc = 0x000;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::LDY_IM, 0x00
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->y, 0x00);
    EXPECT_EQ(nes->cpu->s, 0x02);
}

TEST(LDY, SetNegativeFlag)
{
    System* nes = TestUtils::SetupSystem();

    nes->cpu->pc = 0x000;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::LDY_IM, 0x8c
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->y, 0x8c);
    EXPECT_EQ(nes->cpu->s, 0x80);
}

INSTANTIATE_INSTRUCTION_TEST(
    LDY,
    OpcodeTest { Opcodes::LDY_IM, TestAddressingMode::Immediate, 2 },
    OpcodeTest { Opcodes::LDY_ZP, TestAddressingMode::ZeroPage, 3 },
    OpcodeTest { Opcodes::LDY_ZPX, TestAddressingMode::ZeroPageX, 4 },
    OpcodeTest { Opcodes::LDY_ABS, TestAddressingMode::Absolute, 4 },
    OpcodeTest { Opcodes::LDY_ABX, TestAddressingMode::AbsoluteX, 4 },
    OpcodeTest { Opcodes::LDY_ABX, TestAddressingMode::AbsoluteXPageCrossing, 5 }
);
