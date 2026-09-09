#include <gtest/gtest.h>

#include "test-utils/utils.hpp"
#include "test-utils/instruction-fixture.hpp"

TEST(CPY, SetZeroFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;
    nes->cpu->SetFlag(StatusFlag::Z, false);

    nes->cpu->y = 0x42;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::CPY_IM, 0x42
    });

    nes->cpu->Step();
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
}

TEST(CPY, SetCarryFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;
    nes->cpu->SetFlag(StatusFlag::C, false);

    nes->cpu->y = 0x42;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::CPY_IM, 0x40
    });

    nes->cpu->Step();
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::C));
}

TEST(CPY, SetNegativeFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;

    nes->cpu->y = 0x00;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::CPY_IM, 0x01
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->y, 0x00);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
}

INSTANTIATE_INSTRUCTION_TEST(
    CPY,
    OpcodeTest { Opcodes::CPY_IM, TestAddressingMode::Immediate, 2 },
    OpcodeTest { Opcodes::CPY_ZP, TestAddressingMode::ZeroPage, 3 },
    OpcodeTest { Opcodes::CPY_ABS, TestAddressingMode::Absolute, 4 }
);
