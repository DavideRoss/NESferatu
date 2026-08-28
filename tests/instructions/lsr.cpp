#include <gtest/gtest.h>

#include "test-utils/instruction-fixture.hpp"

TEST(LSR, RightShiftAccumulator)
{
    System* nes = new System();
    nes->cpu->pc = 0x0000;

    nes->cpu->a = 0x42;

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::LSR_ACC
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x21);
}

TEST(LSR, AcccumulatorSetCarryFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x0000;

    nes->cpu->a = 0x01;

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::LSR_ACC
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x00);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::C));
}

TEST(LSR, AcccumulatorSetZeroFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x0000;

    nes->cpu->a = 0x00;

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::LSR_ACC
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x00);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
}

TEST(LSR, AcccumulatorSetNegativeFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x0000;

    nes->cpu->SetFlag(StatusFlag::N, true);
    nes->cpu->a = 0x80;

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::LSR_ACC
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x40);
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::N));
}

TEST(LSR, RightShiftZeroPage)
{
    System* nes = new System();
    nes->cpu->pc = 0x0000;

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::LSR_ZP, 0x20
    });

    nes->mem->Write(0x0020, 0x42);

    nes->cpu->Step();
    EXPECT_EQ(nes->mem->Read(0x0020), 0x21);
}

TEST(LSR, ZeroPageSetCarryFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x0000;

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::LSR_ZP, 0x20
    });

    nes->mem->Write(0x0020, 0x01);

    nes->cpu->Step();
    EXPECT_EQ(nes->mem->Read(0x0020), 0x00);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::C));
}

TEST(LSR, ZeroPageSetZeroFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x0000;

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::LSR_ZP, 0x20
    });

    nes->mem->Write(0x0020, 0x00);

    nes->cpu->Step();
    EXPECT_EQ(nes->mem->Read(0x0020), 0x00);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
}

TEST(LSR, ZeroPageSetNegativeFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x0000;

    nes->cpu->SetFlag(StatusFlag::N, true);

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::LSR_ZP, 0x20
    });

    nes->mem->Write(0x0020, 0x80);

    nes->cpu->Step();
    EXPECT_EQ(nes->mem->Read(0x0020), 0x40);
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::N));
}

INSTANTIATE_INSTRUCTION_TEST(
    LSR,
    OpcodeTest { Opcodes::LSR_ACC, TestAddressingMode::Accumulator, 2 },
    OpcodeTest { Opcodes::LSR_ZP, TestAddressingMode::ZeroPage, 5 },
    OpcodeTest { Opcodes::LSR_ZPX, TestAddressingMode::ZeroPageX, 6 },
    OpcodeTest { Opcodes::LSR_ABS, TestAddressingMode::Absolute, 6 },
    OpcodeTest { Opcodes::LSR_ABX, TestAddressingMode::AbsoluteX, 7 }
);
