#include <gtest/gtest.h>

#include "test-utils/utils.hpp"
#include "test-utils/instruction-fixture.hpp"

TEST(LDX, SetZeroFlag)
{
    System* nes = TestUtils::CreateTestSystem();

    nes->cpu->pc = 0x0000;

    nes->cpu->WriteSpan(0x0000,
    {
        Opcodes::LDX_IM, 0x00
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->x, 0x00);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::N));
}

TEST(LDX, SetNegativeFlag)
{
    System* nes = TestUtils::CreateTestSystem();

    nes->cpu->pc = 0x0000;

    nes->cpu->WriteSpan(0x0000,
    {
        Opcodes::LDX_IM, 0x8c
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->x, 0x8c);
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::Z));
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
}

INSTANTIATE_INSTRUCTION_TEST(
    LDX,
    OpcodeTest { Opcodes::LDX_IM, TestAddressingMode::Immediate, 2 },
    OpcodeTest { Opcodes::LDX_ZP, TestAddressingMode::ZeroPage, 3 },
    OpcodeTest { Opcodes::LDX_ZPY, TestAddressingMode::ZeroPageY, 4 },
    OpcodeTest { Opcodes::LDX_ABS, TestAddressingMode::Absolute, 4 },
    OpcodeTest { Opcodes::LDX_ABY, TestAddressingMode::AbsoluteY, 4 },
    OpcodeTest { Opcodes::LDX_ABY, TestAddressingMode::AbsoluteYPageCrossing, 5 }
);
