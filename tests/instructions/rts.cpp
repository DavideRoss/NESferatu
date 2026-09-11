#include <gtest/gtest.h>

#include "test-utils/utils.hpp"

TEST(RTS, ReturnFromSubroutine)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;
    nes->cpu->sp = 0xfd;

    // Stack
    nes->mem->WriteSeq(0x01fe, { 0x22, 0x01 });

    nes->mem->WriteSeq(0x0000, {
        Opcodes::RTS_IMP
    });

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(nes->cpu->pc, 0x0123);
    EXPECT_EQ(nes->cpu->sp, 0xff);
}
