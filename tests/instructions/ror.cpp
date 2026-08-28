#include "test-utils/instruction-fixture.hpp"

TEST(ROR, RightRotateAccumulator)
{
    System* nes = new System();
    nes->cpu->pc = 0x0000;

    nes->cpu->a = 0x42;

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::ROR_ACC
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x21);
}

TEST(ROR, AcccumulatorGetCarryFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x0000;

    nes->cpu->a = 0x42;
    nes->cpu->SetFlag(StatusFlag::C, true);

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::ROR_ACC
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0xa1);
}

TEST(ROR, AcccumulatorSetCarryFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x0000;

    nes->cpu->a = 0x01;

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::ROR_ACC
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x00);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::C));
}

TEST(ROR, AcccumulatorSetZeroFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x0000;

    nes->cpu->a = 0x00;

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::ROR_ACC
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x00);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
}

TEST(ROR, AcccumulatorSetNegativeFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x0000;

    nes->cpu->a = 0x00;
    nes->cpu->SetFlag(StatusFlag::C, true);

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::ROR_ACC
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x80);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
}

TEST(ROR, RightRotateZeropage)
{
    System* nes = new System();
    nes->cpu->pc = 0x0000;

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::ROR_ZP, 0x20
    });

    nes->mem->Write(0x0020, 0x42);

    nes->cpu->Step();
    EXPECT_EQ(nes->mem->Read(0x0020), 0x21);
}

TEST(ROR, ZeroPageGetCarryFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x0000;

    nes->cpu->SetFlag(StatusFlag::C, true);

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::ROR_ZP, 0x20
    });

    nes->mem->Write(0x0020, 0x42);

    nes->cpu->Step();
    EXPECT_EQ(nes->mem->Read(0x0020), 0xa1);
}

TEST(ROR, ZeroPageSetCarryFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x0000;

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::ROR_ZP, 0x20
    });

    nes->mem->Write(0x0020, 0x01);

    nes->cpu->Step();
    EXPECT_EQ(nes->mem->Read(0x0020), 0x00);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::C));
}

TEST(ROR, ZeroPageSetZeroFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x0000;

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::ROR_ZP, 0x20
    });

    nes->mem->Write(0x0020, 0x00);

    nes->cpu->Step();
    EXPECT_EQ(nes->mem->Read(0x0020), 0x00);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
}

TEST(ROR, ZeroPageSetNegativeFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x0000;

    nes->cpu->SetFlag(StatusFlag::C, true);

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::ROR_ZP, 0x20
    });

    nes->mem->Write(0x0020, 0x00);

    nes->cpu->Step();
    EXPECT_EQ(nes->mem->Read(0x0020), 0x80);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
}

INSTANTIATE_INSTRUCTION_TEST(
    ROR,
    OpcodeTest { Opcodes::ROR_ACC, TestAddressingMode::Accumulator, 2 },
    OpcodeTest { Opcodes::ROR_ZP, TestAddressingMode::ZeroPage, 5 },
    OpcodeTest { Opcodes::ROR_ZPX, TestAddressingMode::ZeroPageX, 6 },
    OpcodeTest { Opcodes::ROR_ABS, TestAddressingMode::Absolute, 6 },
    OpcodeTest { Opcodes::ROR_ABX, TestAddressingMode::AbsoluteX, 7 }
);
