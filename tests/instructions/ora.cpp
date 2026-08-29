#include <gtest/gtest.h>

#include "test-utils/utils.hpp"
#include "test-utils/instruction-fixture.hpp"

TEST(ORA, SetZeroFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;
    nes->cpu->SetFlag(StatusFlag::Z, true);

    nes->cpu->a = 0xf0;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::ORA_IM, 0x0f
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0xff);
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::Z));
}

TEST(ORA, SetNegativeFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;

    nes->cpu->a = 0x0f;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::ORA_IM, 0xf0
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0xff);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
}

INSTANTIATE_INSTRUCTION_TEST(
    ORA,
    OpcodeTest { Opcodes::ORA_IM, TestAddressingMode::Immediate, 2 },
    OpcodeTest { Opcodes::ORA_ZP, TestAddressingMode::ZeroPage, 3 },
    OpcodeTest { Opcodes::ORA_ZPX, TestAddressingMode::ZeroPageX, 4 },
    OpcodeTest { Opcodes::ORA_ABS, TestAddressingMode::Absolute, 4 },
    OpcodeTest { Opcodes::ORA_ABX, TestAddressingMode::AbsoluteX, 4 },
    OpcodeTest { Opcodes::ORA_ABX, TestAddressingMode::AbsoluteXPageCrossing, 5 },
    OpcodeTest { Opcodes::ORA_ABY, TestAddressingMode::AbsoluteY, 4 },
    OpcodeTest { Opcodes::ORA_ABY, TestAddressingMode::AbsoluteYPageCrossing, 5 },
    OpcodeTest { Opcodes::ORA_IDX, TestAddressingMode::IndirectX, 6 },
    OpcodeTest { Opcodes::ORA_IDY, TestAddressingMode::IndirectY, 5 },
    OpcodeTest { Opcodes::ORA_IDY, TestAddressingMode::IndirectYPageCrossing, 6 }
);
