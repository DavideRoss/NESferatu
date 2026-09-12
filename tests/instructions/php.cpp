#include <gtest/gtest.h>

#include "test-utils/utils.hpp"

TEST(PHP, PushProcessorStatus)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;
    nes->cpu->sp = 0xFF;
    nes->cpu->s = StatusFlag::U;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::PHP_IMP
    });

    nes->cpu->SetFlag(StatusFlag::C, false);
    nes->cpu->SetFlag(StatusFlag::Z, false);
    nes->cpu->SetFlag(StatusFlag::N, true);

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 3);

    const u8 status = nes->mem->Read(0x01ff);
    EXPECT_TRUE(status & StatusFlag::N);
    EXPECT_FALSE(status & StatusFlag::Z);
    EXPECT_FALSE(status & StatusFlag::C);
    EXPECT_TRUE(status & StatusFlag::B);
    EXPECT_TRUE(status & StatusFlag::U);
}
