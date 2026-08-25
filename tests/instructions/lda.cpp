#include <gtest/gtest.h>

#include "utils.hpp"

TEST(LDA, Immediate)
{
    System* nes = TestUtils::SetupSystem();

    nes->cpu->pc = 0x000;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::LDA_IM, 0x12
    });

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(nes->cpu->a, 0x12);
    EXPECT_EQ(nes->cpu->s, 0x00);
}

TEST(LDA, ImmediateZeroFlag)
{
    System* nes = TestUtils::SetupSystem();

    nes->cpu->pc = 0x000;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::LDA_IM, 0x00
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x00);
    EXPECT_EQ(nes->cpu->s, 0x02);
}

TEST(LDA, ImmediateNegativeFlag)
{
    System* nes = TestUtils::SetupSystem();

    nes->cpu->pc = 0x000;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::LDA_IM, 0x8c
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x8c);
    EXPECT_EQ(nes->cpu->s, 0x80);
}

TEST(LDA, ZeroPage)
{
    System* nes = TestUtils::SetupSystem();

    nes->cpu->pc = 0x000;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::LDA_ZP, 0x55
    });

    nes->mem->Write(0x0055, 0x12);

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(nes->cpu->a, 0x12);
    EXPECT_EQ(nes->cpu->s, 0x00);
}

TEST(LDA, ZeroPageX)
{
    System* nes = TestUtils::SetupSystem();

    nes->cpu->pc = 0x000;
    nes->cpu->x = 0x0f;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::LDA_ZPX, 0x60
    });

    nes->mem->Write(0x006f, 0x12);

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(nes->cpu->a, 0x12);
    EXPECT_EQ(nes->cpu->s, 0x00);
}

TEST(LDA, Absolute)
{
    System* nes = TestUtils::SetupSystem();

    nes->cpu->pc = 0x000;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::LDA_ABS, 0x22, 0x01
    });

    nes->mem->Write(0x0122, 0x12);

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(nes->cpu->a, 0x12);
    EXPECT_EQ(nes->cpu->s, 0x00);
}

TEST(LDA, AbsoluteX)
{
    System* nes = TestUtils::SetupSystem();

    nes->cpu->pc = 0x0000;
    nes->cpu->x = 0x05;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::LDA_ABX, 0x22, 0x01
    });

    nes->mem->Write(0x0127, 0x12);

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(nes->cpu->a, 0x12);
    EXPECT_EQ(nes->cpu->s, 0x00);
}

TEST(LDA, AbsoluteXPageCrossing)
{
    System* nes = TestUtils::SetupSystem();

    nes->cpu->pc = 0x0000;
    nes->cpu->x = 0xff;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::LDA_ABX, 0x22, 0x01
    });

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 5);
}

TEST(LDA, AbsoluteY)
{
    System* nes = TestUtils::SetupSystem();

    nes->cpu->pc = 0x0000;
    nes->cpu->y = 0x05;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::LDA_ABY, 0x22, 0x01
    });

    nes->mem->Write(0x0127, 0x12);

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(nes->cpu->a, 0x12);
    EXPECT_EQ(nes->cpu->s, 0x00);
}

TEST(LDA, AbsoluteYPageCrossing)
{
    System* nes = TestUtils::SetupSystem();

    nes->cpu->pc = 0x0000;
    nes->cpu->y = 0xff;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::LDA_ABY, 0x22, 0x01
    });

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 5);
}

TEST(LDA, IndirectX)
{
    System* nes = TestUtils::SetupSystem();

    nes->cpu->pc = 0x0000;
    nes->cpu->x = 0x04;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::LDA_IDX, 0x20
    });

    // ($20 + X) = $24
    nes->mem->Write(0x0024, 0xaa);
    nes->mem->Write(0x0025, 0x07);

    // Final data
    nes->mem->Write(0x07aa, 0x12);

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(nes->cpu->a, 0x12);
    EXPECT_EQ(nes->cpu->s, 0x00);
}

TEST(LDA, IndirectXWrapsZeroPage)
{
    System* nes = TestUtils::SetupSystem();

    nes->cpu->pc = 0x0000;
    nes->cpu->x = 0x05;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::LDA_IDX, 0xfe
    });

    // ($fe + X) = $103 -> wraps to $03
    nes->mem->Write(0x0003, 0xaa);
    nes->mem->Write(0x0004, 0x07);

    // Final data
    nes->mem->Write(0x07aa, 0x12);

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x12);
}

TEST(LDA, IndirectY)
{
    System* nes = TestUtils::SetupSystem();

    nes->cpu->pc = 0x0000;
    nes->cpu->y = 0x04;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::LDA_IDY, 0x20
    });

    nes->mem->Write(0x0020, 0xaa);
    nes->mem->Write(0x0021, 0x07);

    // $07aa + Y = $07ae
    nes->mem->Write(0x07ae, 0x12);

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(nes->cpu->a, 0x12);
    EXPECT_EQ(nes->cpu->s, 0x00);
}

TEST(LDA, IndirectYPageCrossing)
{
    System* nes = TestUtils::SetupSystem();

    nes->cpu->pc = 0x0000;
    nes->cpu->y = 0x01;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::LDA_IDY, 0x20
    });

    nes->mem->Write(0x0020, 0xff);
    nes->mem->Write(0x0021, 0x06);

    // $06ff + Y = $0700
    nes->mem->Write(0x0700, 0x12);

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(nes->cpu->a, 0x12);
}
