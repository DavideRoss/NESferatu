#include <gtest/gtest.h>

#include "utils.hpp"

TEST(LDA, Immediate)
{
    System* nes = TestUtils::SetupSystem();

    // Reset program counter
    nes->cpu->pc = 0x000;
    
    // Write instructions
    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::LDA_IM, 0x00,
        Opcodes::LDA_IM, 0x8c
    });

    // Test 1: zero flag
    u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(nes->cpu->a, 0x00);
    EXPECT_EQ(nes->cpu->s, 0x02);

    // Test 2: negative flag
    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x8c);
    EXPECT_EQ(nes->cpu->s, 0x80);
}

TEST(LDA, ZeroPage)
{
    System* nes = TestUtils::SetupSystem();

    // Reset program counter
    nes->cpu->pc = 0x000;

    // Write instructions
    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::LDA_ZP, 0x55,
        Opcodes::LDA_ZP, 0x66
    });

    nes->mem->Write(0x0055, 0x00);
    nes->mem->Write(0x0066, 0x8c);

    // Test 1: zero flag
    u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(nes->cpu->a, 0x00);
    EXPECT_EQ(nes->cpu->s, 0x02);

    // Test 2: negative flag
    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x8c);
    EXPECT_EQ(nes->cpu->s, 0x80);
}

TEST(LDA, ZeroPageX)
{
    System* nes = TestUtils::SetupSystem();

    // Reset program counter
    nes->cpu->pc = 0x000;

    // Set X register
    nes->cpu->x = 0x0f;

    // Write instructions
    nes->mem->WriteSeq(0x0000, {
        Opcodes::LDA_ZPX, 0x60,
        Opcodes::LDA_ZPX, 0x70,
        Opcodes::LDA_ZPX, 0x80,
    });

    nes->mem->Write(0x006f, 0x12);
    nes->mem->Write(0x007f, 0x00);
    nes->mem->Write(0x008f, 0x8c);

    // Test 1: normal read
    u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(nes->cpu->a, 0x12);
    EXPECT_EQ(nes->cpu->s, 0x00);

    // Test 2: zero flag
    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x00);
    EXPECT_EQ(nes->cpu->s, 0x02);

    // Test 3: negative flag
    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x8c);
    EXPECT_EQ(nes->cpu->s, 0x80);
}

TEST(LDA, Absolute)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x000;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::LDA_ABS, 0x22, 0x01,
        Opcodes::LDA_ABS, 0x44, 0x03,
        Opcodes::LDA_ABS, 0x66, 0x05,
    });

    nes->mem->Write(0x0122, 0x12);
    nes->mem->Write(0x0344, 0x00);
    nes->mem->Write(0x0566, 0x8c);

    // Test 1: normal read
    u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(nes->cpu->a, 0x12);
    EXPECT_EQ(nes->cpu->s, 0x00);

    // Test 2: zero flag
    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x00);
    EXPECT_EQ(nes->cpu->s, 0x02);

    // Test 3: negative flag
    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x8c);
    EXPECT_EQ(nes->cpu->s, 0x80);
}

TEST(LDA, AbsoluteX)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x0000;
    nes->cpu->x = 0x05;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::LDA_ABX, 0x22, 0x01,
        Opcodes::LDA_ABX, 0x44, 0x03,
        Opcodes::LDA_ABX, 0x66, 0x05,
    });

    nes->mem->Write(0x0127, 0x12);
    nes->mem->Write(0x0349, 0x00);
    nes->mem->Write(0x056b, 0x8c);

    // Test 1: normal read
    u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(nes->cpu->a, 0x12);
    EXPECT_EQ(nes->cpu->s, 0x00);

    // Test 2: zero flag
    cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(nes->cpu->a, 0x00);
    EXPECT_EQ(nes->cpu->s, 0x02);

    // Test 3: negative flag
    cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(nes->cpu->a, 0x8c);
    EXPECT_EQ(nes->cpu->s, 0x80);

    // Test 4: zero page cycle count
    nes->cpu->x = 0xff;
    nes->cpu->pc = 0x0000;
    cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 5);
}

TEST(LDA, AbsoluteY)
{
    System* nes = TestUtils::SetupSystem();
    nes->cpu->pc = 0x0000;
    nes->cpu->y = 0x05;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::LDA_ABY, 0x22, 0x01,
        Opcodes::LDA_ABY, 0x44, 0x03,
        Opcodes::LDA_ABY, 0x66, 0x05,
    });

    nes->mem->Write(0x0127, 0x12);
    nes->mem->Write(0x0349, 0x00);
    nes->mem->Write(0x056b, 0x8c);

    // Test 1: normal read
    u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(nes->cpu->a, 0x12);
    EXPECT_EQ(nes->cpu->s, 0x00);

    // Test 2: zero flag
    cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(nes->cpu->a, 0x00);
    EXPECT_EQ(nes->cpu->s, 0x02);

    // Test 3: negative flag
    cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(nes->cpu->a, 0x8c);
    EXPECT_EQ(nes->cpu->s, 0x80);

    // Test 4: zero page cycle count
    nes->cpu->y = 0xff;
    nes->cpu->pc = 0x0000;
    cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 5);
}
