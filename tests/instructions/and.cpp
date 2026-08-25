#include <gtest/gtest.h>

#include "utils.hpp"

TEST(AND, Immediate)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->cpu->a = 0xcc;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::AND_IM, 0x7f
    });

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(nes->cpu->a, 0x4c);

    // Checking flags
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::Z));
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::N));
}

TEST(AND, ImmediateZeroFlag)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->cpu->a = 0xf0;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::AND_IM, 0x0f
    });

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(nes->cpu->a, 0x00);

    // Checking flags
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::N));
}

TEST(AND, ImmediateNegativeFlag)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->cpu->a = 0xcc;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::AND_IM, 0xaa
    });

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(nes->cpu->a, 0x88);

    // Checking flags
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::Z));
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
}

TEST(AND, ZeroPage)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::AND_ZP, 0xaa
    });

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 3);
}

TEST(AND, ZeroPageX)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::AND_ZPX, 0xaa
    });

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 4);
}

TEST(AND, Absolute)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::AND_ABS, 0xaa
    });

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 4);
}

TEST(AND, AbsoluteX)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::AND_ABX, 0xaa, 0x06
    });

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 4);
}

TEST(AND, AbsoluteXPageCrossing)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;
    nes->cpu->x = 0x01;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::AND_ABX, 0xff, 0x06
    });

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 5);
}

TEST(AND, AbsoluteY)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::AND_ABY, 0xaa, 0x06
    });

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 4);
}

TEST(AND, AbsoluteYPageCrossing)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;
    nes->cpu->y = 0x01;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::AND_ABY, 0xff, 0x06
    });

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 5);
}

TEST(AND, IndirectX)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::AND_IDX, 0x20
    });

    nes->mem->Write(0x0020, 0xaa);
    nes->mem->Write(0x0021, 0x06);

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 6);
}

TEST(AND, IndirectY)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::AND_IDY, 0x20
    });

    nes->mem->Write(0x0020, 0xaa);
    nes->mem->Write(0x0021, 0x06);

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 5);
}

TEST(AND, IndirectYPageCrossing)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;
    nes->cpu->y = 0x01;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::AND_IDY, 0x20
    });

    nes->mem->Write(0x0020, 0xff);
    nes->mem->Write(0x0021, 0x06);

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 6);
}
