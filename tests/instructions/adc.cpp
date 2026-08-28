#include <gtest/gtest.h>

#include "test-utils/instruction-fixture.hpp"

TEST(ADC, SetZeroFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x0000;

    nes->cpu->a = 0x00;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::ADC_IM, 0x00
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x00);

    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::C));
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::N));
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::V));
}

TEST(ADC, SetCarryOutFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x0000;

    nes->cpu->a = 0xff;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::ADC_IM, 0x01
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x00);

    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::C));
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::N));
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::V));
}

TEST(ADC, UsesCarryInFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x0000;

    nes->cpu->a = 0x10;
    nes->cpu->SetFlag(StatusFlag::C, true);

    nes->mem->WriteSeq(0x0000, {
        Opcodes::ADC_IM, 0x20
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x31);
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::C));
}

TEST(ADC, SetNegativeFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x0000;

    nes->cpu->a = 0x40;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::ADC_IM, 0x40
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x80);

    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::V));
}

TEST(ADC, SetOverflowFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x0000;

    nes->cpu->a = 0x7f;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::ADC_IM, 0x01
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x80);

    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::C));
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::Z));
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::V));
}

TEST(ADC, SetOverflowFlagForNegative)
{
    System* nes = new System();
    nes->cpu->pc = 0x0000;

    nes->cpu->a = 0x80;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::ADC_IM, 0xff
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x7f);

    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::C));
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::Z));
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::N));
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::V));
}

INSTANTIATE_INSTRUCTION_TEST(
    ADC,
    OpcodeTest { Opcodes::ADC_IM, TestAddressingMode::Immediate, 2 },
    OpcodeTest { Opcodes::ADC_ZP, TestAddressingMode::ZeroPage, 3 },
    OpcodeTest { Opcodes::ADC_ZPX, TestAddressingMode::ZeroPageX, 4 },
    OpcodeTest { Opcodes::ADC_ABS, TestAddressingMode::Absolute, 4 },
    OpcodeTest { Opcodes::ADC_ABX, TestAddressingMode::AbsoluteX, 4 },
    OpcodeTest { Opcodes::ADC_ABX, TestAddressingMode::AbsoluteXPageCrossing, 5 },
    OpcodeTest { Opcodes::ADC_ABY, TestAddressingMode::AbsoluteY, 4 },
    OpcodeTest { Opcodes::ADC_ABY, TestAddressingMode::AbsoluteYPageCrossing, 5 },
    OpcodeTest { Opcodes::ADC_IDX, TestAddressingMode::IndirectX, 6 },
    OpcodeTest { Opcodes::ADC_IDY, TestAddressingMode::IndirectY, 5 },
    OpcodeTest { Opcodes::ADC_IDY, TestAddressingMode::IndirectYPageCrossing, 6 }
);
