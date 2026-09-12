#include <gtest/gtest.h>

#include "test-utils/utils.hpp"

TEST(PHA, PushAccumulator)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;
    nes->cpu->sp = 0xff;
    nes->cpu->a = 0x42;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::PHA_IMP
    });

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(nes->mem->Read(0x01ff), 0x42);
}
