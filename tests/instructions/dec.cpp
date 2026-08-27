#include <gtest/gtest.h>

#include "test-utils/utils.hpp"
#include "test-utils/instruction-fixture.hpp"

TEST(DEC, BasicDecrement)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::DEC_ZP, 0x10
    });

    nes->mem->Write(0x0010, 0x42);

    nes->cpu->Step();
    EXPECT_EQ(nes->mem->Read(0x0010), 0x41);
}

TEST(DEC, SetZeroFlag)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::DEC_ZP, 0x10
    });

    nes->mem->Write(0x0010, 0x01);

    nes->cpu->Step();
    EXPECT_EQ(nes->mem->Read(0x0010), 0x00);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
}

TEST(DEC, SetNegativeFlag)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::DEC_ZP, 0x10
    });

    nes->mem->Write(0x0010, 0x00);

    nes->cpu->Step();
    EXPECT_EQ(nes->mem->Read(0x0010), 0xff);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
}

INSTANTIATE_INSTRUCTION_TEST(
    DEC,
    OpcodeTest { Opcodes::DEC_ZP, TestAddressingMode::ZeroPage, 5 },
    OpcodeTest { Opcodes::DEC_ZPX, TestAddressingMode::ZeroPageX, 6 },
    OpcodeTest { Opcodes::DEC_ABS, TestAddressingMode::Absolute, 6 },
    OpcodeTest { Opcodes::DEC_ABX, TestAddressingMode::AbsoluteX, 7 }
);
