#include <gtest/gtest.h>

#include "test-utils/utils.hpp"

TEST(SEI, SetInterruptDisableFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;

    nes->mem->Write(0x0000, Opcodes::SEI_IMP);

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 2);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::I));
}
