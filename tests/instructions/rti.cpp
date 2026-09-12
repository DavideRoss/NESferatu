#include <gtest/gtest.h>

#include "test-utils/utils.hpp"

TEST(RTI, ReturnFromInterrupt)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;
    nes->cpu->sp = 0xFC;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::RTI_IMP
    });

    // Stack
    nes->mem->Write(0x01FD, 0xa2);
    nes->mem->Write(0x01FE, 0x23);
    nes->mem->Write(0x01FF, 0x01);

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 6);
    EXPECT_EQ(nes->cpu->pc, 0x0123);
    EXPECT_EQ(nes->cpu->sp, 0xff);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
}
