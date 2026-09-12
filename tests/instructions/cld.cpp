#include <gtest/gtest.h>

#include "test-utils/utils.hpp"

TEST(CLD, ClearDecimalModeFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;
    nes->cpu->SetFlag(StatusFlag::D, true);

    nes->mem->Write(0x0000, Opcodes::CLD_IMP);

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 2);
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::D));
}
