#include <gtest/gtest.h>

#include "test-utils/instruction-fixture.hpp"

TEST(ASL, LeftShiftAccumulator)
{
    System* nes = new System();
    nes->cpu->pc = 0x0000;

    nes->cpu->a = 0x21;

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::ASL_ACC
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x42);
}

TEST(ASL, AcccumulatorSetCarryFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x0000;

    nes->cpu->a = 0x80;

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::ASL_ACC
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x00);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::C));
}

TEST(ASL, AcccumulatorSetZeroFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x0000;

    nes->cpu->a = 0x00;

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::ASL_ACC
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x00);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
}

TEST(ASL, AcccumulatorSetNegativeFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x0000;

    nes->cpu->a = 0x40;

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::ASL_ACC
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x80);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
}

TEST(ASL, LeftShiftZeroPage)
{
    System* nes = new System();
    nes->cpu->pc = 0x0000;

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::ASL_ZP, 0x20
    });

    nes->mem->Write(0x0020, 0x21);

    nes->cpu->Step();
    EXPECT_EQ(nes->mem->Read(0x0020), 0x42);
}

TEST(ASL, ZeroPageSetCarryFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x0000;

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::ASL_ZP, 0x20
    });

    nes->mem->Write(0x0020, 0x80);

    nes->cpu->Step();
    EXPECT_EQ(nes->mem->Read(0x0020), 0x00);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::C));
}

TEST(ASL, ZeroPageSetZeroFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x0000;

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::ASL_ZP, 0x20
    });

    nes->mem->Write(0x0020, 0x00);

    nes->cpu->Step();
    EXPECT_EQ(nes->mem->Read(0x0020), 0x00);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
}

TEST(ASL, ZeroPageSetNegativeFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x0000;

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::ASL_ZP, 0x20
    });

    nes->mem->Write(0x0020, 0x40);

    nes->cpu->Step();
    EXPECT_EQ(nes->mem->Read(0x0020), 0x80);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
}

INSTANTIATE_INSTRUCTION_TEST(
    ASL,
    OpcodeTest { Opcodes::ASL_ACC, TestAddressingMode::Accumulator, 2 },
    OpcodeTest { Opcodes::ASL_ZP, TestAddressingMode::ZeroPage, 5 },
    OpcodeTest { Opcodes::ASL_ZPX, TestAddressingMode::ZeroPageX, 6 },
    OpcodeTest { Opcodes::ASL_ABS, TestAddressingMode::Absolute, 6 },
    OpcodeTest { Opcodes::ASL_ABX, TestAddressingMode::AbsoluteX, 7 }
);
