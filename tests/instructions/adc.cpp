#include <gtest/gtest.h>

#include "utils.hpp"

// TODO: no need to test functionality over and over again
// Just test in immediate mode, and check the cycle count for the rest

TEST(ADC, Immediate)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->cpu->a = 0x10;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::ADC_IM, 0x20
    });

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(nes->cpu->a, 0x30);

    // Checking flags
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::C));
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::Z));
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::N));
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::V));
}

TEST(ADC, SetZeroFlag)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->cpu->a = 0x00;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::ADC_IM, 0x00
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x00);

    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::C));
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::N));
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::V));
}

TEST(ADC, SetCarryOutFlag)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->cpu->a = 0xff;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::ADC_IM, 0x01
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x00);

    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::C));
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::N));
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::V));
}

TEST(ADC, UsesCarryInFlag)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->cpu->a = 0x10;
    nes->cpu->SetFlag(StatusFlag::C, true);

    nes->mem->WriteSeq(0x0000, {
        Opcodes::ADC_IM, 0x20
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x31);
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::C));
}

TEST(ADC, SetNegativeFlag)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->cpu->a = 0x40;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::ADC_IM, 0x40
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x80);

    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::V));
}

TEST(ADC, SetOverflowFlag)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->cpu->a = 0x7f;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::ADC_IM, 0x01
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x80);

    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::C));
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::Z));
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::V));
}

TEST(ADC, SetOverflowFlagForNegative)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->cpu->a = 0x80;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::ADC_IM, 0xff
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x7f);

    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::C));
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::Z));
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::N));
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::V));
}

TEST(ADC, ZeroPage)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->cpu->a = 0x10;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::ADC_ZP, 0x20
    });

    nes->mem->Write(0x0020, 0x20);

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(nes->cpu->a, 0x30);
}

TEST(ADC, ZeroPageX)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->cpu->a = 0x10;
    nes->cpu->x = 0x05;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::ADC_ZPX, 0x20
    });

    nes->mem->Write(0x0025, 0x20);

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(nes->cpu->a, 0x30);
}

TEST(ADC, Absolute)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->cpu->a = 0x10;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::ADC_ABS, 0xaa, 0x07
    });

    nes->mem->Write(0x07aa, 0x20);

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(nes->cpu->a, 0x30);
}

TEST(ADC, AbsoluteX)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->cpu->a = 0x10;
    nes->cpu->x = 0x05;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::ADC_ABX, 0xaa, 0x07
    });

    nes->mem->Write(0x07af, 0x20);

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(nes->cpu->a, 0x30);
}

TEST(ADC, AbsoluteXPageCrossing)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->cpu->a = 0x10;
    nes->cpu->x = 0x01;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::ADC_ABX, 0xff, 0x06
    });

    nes->mem->Write(0x0700, 0x20);

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(nes->cpu->a, 0x30);
}

TEST(ADC, AbsoluteY)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->cpu->a = 0x10;
    nes->cpu->y = 0x05;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::ADC_ABY, 0xaa, 0x07
    });

    nes->mem->Write(0x07af, 0x20);

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(nes->cpu->a, 0x30);
}

TEST(ADC, AbsoluteYPageCrossing)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->cpu->a = 0x10;
    nes->cpu->y = 0x01;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::ADC_ABY, 0xff, 0x06
    });

    nes->mem->Write(0x0700, 0x20);

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(nes->cpu->a, 0x30);
}

TEST(ADC, IndirectX)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->cpu->a = 0x10;
    nes->cpu->x = 0x04;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::ADC_IDX, 0x20
    });

    nes->mem->Write(0x0024, 0xaa);
    nes->mem->Write(0x0025, 0x07);
    nes->mem->Write(0x07aa, 0x20);

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(nes->cpu->a, 0x30);
}

TEST(ADC, IndirectY)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->cpu->a = 0x10;
    nes->cpu->y = 0x05;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::ADC_IDY, 0x20
    });

    nes->mem->Write(0x0020, 0xaa);
    nes->mem->Write(0x0021, 0x07);
    nes->mem->Write(0x07af, 0x20);

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(nes->cpu->a, 0x30);
}

TEST(ADC, IndirectYPageCrossing)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->cpu->a = 0x10;
    nes->cpu->y = 0x01;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::ADC_IDY, 0x20
    });

    nes->mem->Write(0x0020, 0xff);
    nes->mem->Write(0x0021, 0x06);
    nes->mem->Write(0x0700, 0x20);

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(nes->cpu->a, 0x30);
}
