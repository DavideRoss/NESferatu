#include <gtest/gtest.h>

#include "test-utils/utils.hpp"

TEST(CLC, ClearCarryFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;
    nes->cpu->SetFlag(StatusFlag::C, true);

    nes->cpu->WriteByte(0x0000, Opcodes::CLC_IMP);

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 2);
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::C));
}
