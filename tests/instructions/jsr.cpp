#include <gtest/gtest.h>

#include "test-utils/utils.hpp"

TEST(JSR, JumpToSubroutine)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0123;
    nes->cpu->sp = 0xff;

    nes->mem->WriteSeq(0x0123, {
        Opcodes::JSR_ABS, 0x34, 0x12
    });

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(nes->cpu->pc, 0x1234);
    EXPECT_EQ(nes->cpu->sp, 0xfd);

    // Check stack
    EXPECT_EQ(nes->mem->Read(0x01ff), 0x01);
    EXPECT_EQ(nes->mem->Read(0x01fe), 0x25);
}
