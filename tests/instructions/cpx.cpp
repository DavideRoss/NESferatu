#include <gtest/gtest.h>

#include "test-utils/utils.hpp"
#include "test-utils/instruction-fixture.hpp"

TEST(CPX, SetZeroFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;
    nes->cpu->SetFlag(StatusFlag::Z, false);

    nes->cpu->x = 0x42;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::CPX_IM, 0x42
    });

    nes->cpu->Step();
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
}

TEST(CPX, SetCarryFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;
    nes->cpu->SetFlag(StatusFlag::C, false);

    nes->cpu->x = 0x42;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::CPX_IM, 0x40
    });

    nes->cpu->Step();
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::C));
}

TEST(CPX, SetNegativeFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;

    nes->cpu->x = 0x00;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::CPX_IM, 0x01
    });

    nes->cpu->Step();
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
}

INSTANTIATE_INSTRUCTION_TEST(
    CPX,
    OpcodeTest { Opcodes::CPX_IM, TestAddressingMode::Immediate, 2 },
    OpcodeTest { Opcodes::CPX_ZP, TestAddressingMode::ZeroPage, 3 },
    OpcodeTest { Opcodes::CPX_ABS, TestAddressingMode::Absolute, 4 }
);
