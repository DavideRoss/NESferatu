#include <gtest/gtest.h>

#include "test-utils/utils.hpp"
#include "test-utils/instruction-fixture.hpp"

TEST(AND, SetZeroFlag)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->cpu->a = 0xf0;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::AND_IM, 0x0f
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x00);

    // Checking flags
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::N));
}

TEST(AND, SetNegativeFlag)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->cpu->a = 0xcc;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::AND_IM, 0xaa
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x88);

    // Checking flags
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::Z));
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
}

INSTANTIATE_INSTRUCTION_TEST(
    AND,
    OpcodeTest { Opcodes::AND_IM, TestAddressingMode::Immediate, 2 },
    OpcodeTest { Opcodes::AND_ZP, TestAddressingMode::ZeroPage, 3 },
    OpcodeTest { Opcodes::AND_ZPX, TestAddressingMode::ZeroPageX, 4 },
    OpcodeTest { Opcodes::AND_ABS, TestAddressingMode::Absolute, 4 },
    OpcodeTest { Opcodes::AND_ABX, TestAddressingMode::AbsoluteX, 4 },
    OpcodeTest { Opcodes::AND_ABX, TestAddressingMode::AbsoluteXPageCrossing, 5 },
    OpcodeTest { Opcodes::AND_ABY, TestAddressingMode::AbsoluteY, 4 },
    OpcodeTest { Opcodes::AND_ABY, TestAddressingMode::AbsoluteYPageCrossing, 5 },
    OpcodeTest { Opcodes::AND_IDX, TestAddressingMode::IndirectX, 6 },
    OpcodeTest { Opcodes::AND_IDY, TestAddressingMode::IndirectY, 5 },
    OpcodeTest { Opcodes::AND_IDY, TestAddressingMode::IndirectYPageCrossing, 6 }
);
