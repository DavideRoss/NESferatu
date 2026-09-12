#include <gtest/gtest.h>

#include "test-utils/utils.hpp"

TEST(TXS, TransferXToStackPointer)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;
    nes->cpu->x = 0x42;
    nes->cpu->sp = 0xff;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::TXS_IMP
    });

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(nes->cpu->sp, 0x42);
}
