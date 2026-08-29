#include <gtest/gtest.h>

#include "test-utils/utils.hpp"
#include "test-utils/instruction-fixture.hpp"

TEST(INC, BasicIncrement)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::INC_ZP, 0x10
    });

    nes->mem->Write(0x0010, 0x42);

    nes->cpu->Step();
    EXPECT_EQ(nes->mem->Read(0x0010), 0x43);
}

TEST(INC, SetZeroFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::INC_ZP, 0x10
    });

    nes->mem->Write(0x0010, 0xff);

    nes->cpu->Step();
    EXPECT_EQ(nes->mem->Read(0x0010), 0x00);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
}

TEST(INC, SetNegativeFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::INC_ZP, 0x10
    });

    nes->mem->Write(0x0010, 0x7f);

    nes->cpu->Step();
    EXPECT_EQ(nes->mem->Read(0x0010), 0x80);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
}

INSTANTIATE_INSTRUCTION_TEST(
    INC,
    OpcodeTest { Opcodes::INC_ZP, TestAddressingMode::ZeroPage, 5 },
    OpcodeTest { Opcodes::INC_ZPX, TestAddressingMode::ZeroPageX, 6 },
    OpcodeTest { Opcodes::INC_ABS, TestAddressingMode::Absolute, 6 },
    OpcodeTest { Opcodes::INC_ABX, TestAddressingMode::AbsoluteX, 7 }
);
