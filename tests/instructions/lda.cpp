#include <gtest/gtest.h>

#include "test-utils/instruction-fixture.hpp"

TEST(LDA, SetZeroFlag)
{
    System* nes = new System();

    nes->cpu->pc = 0x0000;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::LDA_IM, 0x00
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x00);
    EXPECT_EQ(nes->cpu->s, 0x02);
}

TEST(LDA, SetNegativeFlag)
{
    System* nes = new System();

    nes->cpu->pc = 0x0000;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::LDA_IM, 0x8c
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x8c);
    EXPECT_EQ(nes->cpu->s, 0x80);
}

INSTANTIATE_INSTRUCTION_TEST(
    LDA,
    OpcodeTest { Opcodes::LDA_IM, TestAddressingMode::Immediate, 2 },
    OpcodeTest { Opcodes::LDA_ZP, TestAddressingMode::ZeroPage, 3 },
    OpcodeTest { Opcodes::LDA_ZPX, TestAddressingMode::ZeroPageX, 4 },
    OpcodeTest { Opcodes::LDA_ABS, TestAddressingMode::Absolute, 4 },
    OpcodeTest { Opcodes::LDA_ABX, TestAddressingMode::AbsoluteX, 4 },
    OpcodeTest { Opcodes::LDA_ABX, TestAddressingMode::AbsoluteXPageCrossing, 5 },
    OpcodeTest { Opcodes::LDA_ABY, TestAddressingMode::AbsoluteY, 4 },
    OpcodeTest { Opcodes::LDA_ABY, TestAddressingMode::AbsoluteYPageCrossing, 5 },
    OpcodeTest { Opcodes::LDA_IDX, TestAddressingMode::IndirectX, 6 },
    OpcodeTest { Opcodes::LDA_IDY, TestAddressingMode::IndirectY, 5 },
    OpcodeTest { Opcodes::LDA_IDY, TestAddressingMode::IndirectYPageCrossing, 6 }
);
