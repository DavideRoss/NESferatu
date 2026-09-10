#include <gtest/gtest.h>

#include "test-utils/utils.hpp"

TEST(BCS, BranchTaken)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;
    nes->cpu->SetFlag(StatusFlag::C, true);

    nes->mem->WriteSeq(0x0000, {
        Opcodes::BCS_REL, 0x02
    });

    const u32 cycles = nes->cpu->Step();

    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(nes->cpu->pc, 0x0004);
}

TEST(BCS, BranchNotTaken)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;
    nes->cpu->SetFlag(StatusFlag::C, false);

    nes->mem->WriteSeq(0x0000, {
        Opcodes::BCS_REL, 0x02
    });

    const u32 cycles = nes->cpu->Step();

    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(nes->cpu->pc, 0x0002);
}

TEST(BCS, BranchBackward)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0102;
    nes->cpu->SetFlag(StatusFlag::C, true);

    nes->mem->WriteSeq(0x0102, {
        Opcodes::BCS_REL, 0xfe
    });

    const u32 cycles = nes->cpu->Step();

    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(nes->cpu->pc, 0x0102);
}

TEST(BCS, BranchPageCrossing)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x00fe;
    nes->cpu->SetFlag(StatusFlag::C, true);

    nes->mem->WriteSeq(0x00fe, {
        Opcodes::BCS_REL, 0x02
    });

    const u32 cycles = nes->cpu->Step();

    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(nes->cpu->pc, 0x0102);
}
