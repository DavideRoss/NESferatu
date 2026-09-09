#include <gtest/gtest.h>

#include "test-utils/utils.hpp"
#include "test-utils/instruction-fixture.hpp"

TEST(EOR, SetZeroFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;

    nes->cpu->a = 0xff;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::EOR_IM, 0xff
    });

    nes->cpu->Step();
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
}

TEST(EOR, SetNegativeFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;

    nes->cpu->a = 0x0f;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::EOR_IM, 0xf0
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0xff);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
}

INSTANTIATE_INSTRUCTION_TEST(
    EOR,
    OpcodeTest { Opcodes::EOR_IM, TestAddressingMode::Immediate, 2 },
    OpcodeTest { Opcodes::EOR_ZP, TestAddressingMode::ZeroPage, 3 },
    OpcodeTest { Opcodes::EOR_ZPX, TestAddressingMode::ZeroPageX, 4 },
    OpcodeTest { Opcodes::EOR_ABS, TestAddressingMode::Absolute, 4 },
    OpcodeTest { Opcodes::EOR_ABX, TestAddressingMode::AbsoluteX, 4 },
    OpcodeTest { Opcodes::EOR_ABX, TestAddressingMode::AbsoluteXPageCrossing, 5 },
    OpcodeTest { Opcodes::EOR_ABY, TestAddressingMode::AbsoluteY, 4 },
    OpcodeTest { Opcodes::EOR_ABY, TestAddressingMode::AbsoluteYPageCrossing, 5 },
    OpcodeTest { Opcodes::EOR_IDX, TestAddressingMode::IndirectX, 6 },
    OpcodeTest { Opcodes::EOR_IDY, TestAddressingMode::IndirectY, 5 },
    OpcodeTest { Opcodes::EOR_IDY, TestAddressingMode::IndirectYPageCrossing, 6 }
);
