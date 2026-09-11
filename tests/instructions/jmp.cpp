#include <gtest/gtest.h>

#include "test-utils/utils.hpp"

TEST(JMP, Absolute)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::JMP_ABS, 0x23, 0x01
    });

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(nes->cpu->pc, 0x0123);
}

TEST(JMP, Indirect)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::JMP_IND, 0x23, 0x01
    });

    nes->mem->WriteSeq(0x0123, {
        0x21, 0x03
    });

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 5);
    EXPECT_EQ(nes->cpu->pc, 0x0321);
}

TEST(JMP, IndirectPageBoundary)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::JMP_IND, 0xFF, 0x01
    });

    nes->mem->Write(0x01FF, 0xCD);
    nes->mem->Write(0x0100, 0xAB);
    nes->mem->Write(0x0200, 0xEF);

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->pc, 0xABCD);
}
