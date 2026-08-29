#include "test-utils/utils.hpp"
#include "test-utils/instruction-fixture.hpp"

TEST(ROL, LeftRotateAccumulator)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;

    nes->cpu->a = 0x21;

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::ROL_ACC
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x42);
}

TEST(ROL, AcccumulatorGetCarryFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;

    nes->cpu->a = 0x42;
    nes->cpu->SetFlag(StatusFlag::C, true);

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::ROL_ACC
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x85);
}

TEST(ROL, AcccumulatorSetCarryFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;

    nes->cpu->a = 0x80;

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::ROL_ACC
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x00);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::C));
}

TEST(ROL, AcccumulatorSetZeroFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;

    nes->cpu->a = 0x00;

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::ROL_ACC
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x00);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
}

TEST(ROL, AcccumulatorSetNegativeFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;

    nes->cpu->a = 0x40;

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::ROL_ACC
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x80);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
}

TEST(ROL, LeftRotateZeropage)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::ROL_ZP, 0x20
    });

    nes->mem->Write(0x0020, 0x21);

    nes->cpu->Step();
    EXPECT_EQ(nes->mem->Read(0x0020), 0x42);
}

TEST(ROL, ZeroPageGetCarryFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;

    nes->cpu->SetFlag(StatusFlag::C, true);

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::ROL_ZP, 0x20
    });

    nes->mem->Write(0x0020, 0x42);

    nes->cpu->Step();
    EXPECT_EQ(nes->mem->Read(0x0020), 0x85);
}

TEST(ROL, ZeroPageSetCarryFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::ROL_ZP, 0x20
    });

    nes->mem->Write(0x0020, 0x80);

    nes->cpu->Step();
    EXPECT_EQ(nes->mem->Read(0x0020), 0x00);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::C));
}

TEST(ROL, ZeroPageSetZeroFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::ROL_ZP, 0x20
    });

    nes->mem->Write(0x0020, 0x00);

    nes->cpu->Step();
    EXPECT_EQ(nes->mem->Read(0x0020), 0x00);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
}

TEST(ROL, ZeroPageSetNegativeFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;

    nes->mem->WriteSeq(0x0000, 
    {
        Opcodes::ROL_ZP, 0x20
    });

    nes->mem->Write(0x0020, 0x40);

    nes->cpu->Step();
    EXPECT_EQ(nes->mem->Read(0x0020), 0x80);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
}

INSTANTIATE_INSTRUCTION_TEST(
    ROL,
    OpcodeTest { Opcodes::ROL_ACC, TestAddressingMode::Accumulator, 2 },
    OpcodeTest { Opcodes::ROL_ZP, TestAddressingMode::ZeroPage, 5 },
    OpcodeTest { Opcodes::ROL_ZPX, TestAddressingMode::ZeroPageX, 6 },
    OpcodeTest { Opcodes::ROL_ABS, TestAddressingMode::Absolute, 6 },
    OpcodeTest { Opcodes::ROL_ABX, TestAddressingMode::AbsoluteX, 7 }
);
