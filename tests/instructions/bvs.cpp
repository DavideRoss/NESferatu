#include <gtest/gtest.h>

#include "test-utils/utils.hpp"

TEST(BVS, BranchTaken)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;
    nes->cpu->SetFlag(StatusFlag::V, true);

    nes->mem->WriteSeq(0x0000, {
        Opcodes::BVS_REL, 0x02
    });

    const u32 cycles = nes->cpu->Step();

    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(nes->cpu->pc, 0x0004);
}

TEST(BVS, BranchNotTaken)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;
    nes->cpu->SetFlag(StatusFlag::V, false);

    nes->mem->WriteSeq(0x0000, {
        Opcodes::BVS_REL, 0x02
    });

    const u32 cycles = nes->cpu->Step();

    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(nes->cpu->pc, 0x0002);
}

TEST(BVS, BranchBackward)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0102;
    nes->cpu->SetFlag(StatusFlag::V, true);

    nes->mem->WriteSeq(0x0102, {
        Opcodes::BVS_REL, 0xfe
    });

    const u32 cycles = nes->cpu->Step();

    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(nes->cpu->pc, 0x0102);
}

TEST(BVS, BranchPageCrossing)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x00fd;
    nes->cpu->SetFlag(StatusFlag::V, true);

    nes->mem->WriteSeq(0x00fd, {
        Opcodes::BVS_REL, 0x02
    });

    const u32 cycles = nes->cpu->Step();

    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(nes->cpu->pc, 0x0101);
}
