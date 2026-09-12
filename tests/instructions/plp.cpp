#include <gtest/gtest.h>

#include "test-utils/utils.hpp"

TEST(PLP, PullProcessorStatus)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;
    nes->cpu->sp = 0xFE;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::PLP_IMP
    });

    const u8 status =
        StatusFlag::N |
        StatusFlag::V |
        StatusFlag::D |
        StatusFlag::C;

    nes->mem->Write(0x01ff, status);

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 4);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::V));
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::D));
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::C));
}
