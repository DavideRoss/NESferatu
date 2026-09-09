#include <gtest/gtest.h>

#include "test-utils/utils.hpp"
#include "test-utils/instruction-fixture.hpp"

TEST(CMP, SetZeroFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;

    nes->cpu->a = 0x42;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::CMP_IM, 0x42
    });

    nes->cpu->Step();
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
}

TEST(CMP, SetCarryFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;
    nes->cpu->SetFlag(StatusFlag::C, false);

    nes->cpu->a = 0x42;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::CMP_IM, 0x40
    });

    nes->cpu->Step();
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::C));
}

TEST(CMP, SetNegativeFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;

    nes->cpu->a = 0x00;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::CMP_IM, 0x01
    });

    nes->cpu->Step();
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
}

INSTANTIATE_INSTRUCTION_TEST(
    CMP,
    OpcodeTest { Opcodes::CMP_IM, TestAddressingMode::Immediate, 2 },
    OpcodeTest { Opcodes::CMP_ZP, TestAddressingMode::ZeroPage, 3 },
    OpcodeTest { Opcodes::CMP_ZPX, TestAddressingMode::ZeroPageX, 4 },
    OpcodeTest { Opcodes::CMP_ABS, TestAddressingMode::Absolute, 4 },
    OpcodeTest { Opcodes::CMP_ABX, TestAddressingMode::AbsoluteX, 4 },
    OpcodeTest { Opcodes::CMP_ABX, TestAddressingMode::AbsoluteXPageCrossing, 5 },
    OpcodeTest { Opcodes::CMP_ABY, TestAddressingMode::AbsoluteY, 4 },
    OpcodeTest { Opcodes::CMP_ABY, TestAddressingMode::AbsoluteYPageCrossing, 5 },
    OpcodeTest { Opcodes::CMP_IDX, TestAddressingMode::IndirectX, 6 },
    OpcodeTest { Opcodes::CMP_IDY, TestAddressingMode::IndirectY, 5 },
    OpcodeTest { Opcodes::CMP_IDY, TestAddressingMode::IndirectYPageCrossing, 6 }
);
