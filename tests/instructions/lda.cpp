#include <gtest/gtest.h>

#include "utils.hpp"

TEST(LDA, Immediate)
{
    System* nes = TestUtils::SetupSystem();

    // Reset program counter
    nes->cpu->pc = 0x000;
    
    // Write instructions
    nes->mem->Write(0x0000, 0xa9);
    nes->mem->Write(0x0001, 0x00);
    nes->mem->Write(0x0002, 0xa9);
    nes->mem->Write(0x0003, 0x8c);

    // Test 1: zero flag
    nes->Execute(2);
    EXPECT_EQ(nes->cpu->a, 0x00);
    EXPECT_EQ(nes->cpu->s, 0x02);

    // Test 2: negative flag
    nes->Execute(2);
    EXPECT_EQ(nes->cpu->a, 0x8c);
    EXPECT_EQ(nes->cpu->s, 0x80);
}

TEST(LDA, ZeroPage)
{
    System* nes = TestUtils::SetupSystem();

    // Reset program counter
    nes->cpu->pc = 0x000;

    // Write instructions
    nes->mem->Write(0x0000, 0xa5);
    nes->mem->Write(0x0001, 0x55);
    nes->mem->Write(0x0002, 0xa5);
    nes->mem->Write(0x0003, 0x66);
    nes->mem->Write(0x0055, 0x00);
    nes->mem->Write(0x0066, 0x8c);

    // Test 1: zero flag
    nes->Execute(3);
    EXPECT_EQ(nes->cpu->a, 0x00);
    EXPECT_EQ(nes->cpu->s, 0x02);

    // Test 2: negative flag
    nes->Execute(3);
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
    nes->mem->Write(0x0000, 0xb5);
    nes->mem->Write(0x0001, 0x60);
    nes->mem->Write(0x0002, 0xb5);
    nes->mem->Write(0x0003, 0x70);
    nes->mem->Write(0x0004, 0xb5);
    nes->mem->Write(0x0005, 0x80);

    nes->mem->Write(0x006f, 0x12);
    nes->mem->Write(0x007f, 0x00);
    nes->mem->Write(0x008f, 0x8c);

    // Test 1: normal read
    nes->Execute(4);
    EXPECT_EQ(nes->cpu->a, 0x12);
    EXPECT_EQ(nes->cpu->s, 0x00);

    // Test 2: zero flag
    nes->Execute(4);
    EXPECT_EQ(nes->cpu->a, 0x00);
    EXPECT_EQ(nes->cpu->s, 0x02);

    // Test 3: negative flag
    nes->Execute(4);
    EXPECT_EQ(nes->cpu->a, 0x8c);
    EXPECT_EQ(nes->cpu->s, 0x80);
}
