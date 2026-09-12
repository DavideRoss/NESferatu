#include <gtest/gtest.h>

#include "test-utils/utils.hpp"

TEST(TSX, TransferStackPointerToX)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;
    nes->cpu->sp = 0x42;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::TSX_IMP
    });

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(nes->cpu->x, 0x42);
}

TEST(TSX, SetZeroFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;
    nes->cpu->sp = 0x00;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::TSX_IMP
    });

    nes->cpu->Step();
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
}

TEST(TSX, SetNegativeFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;
    nes->cpu->sp = 0x80;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::TSX_IMP
    });

    nes->cpu->Step();
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
}
