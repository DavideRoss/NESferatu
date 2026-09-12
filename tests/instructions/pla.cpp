#include <gtest/gtest.h>

#include "test-utils/utils.hpp"

TEST(PLA, PullAccumulator)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;
    nes->cpu->sp = 0xfe;
    nes->cpu->a = 0x00;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::PLA_IMP
    });

    nes->mem->Write(0x01ff, 0x42);

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(nes->cpu->a, 0x42);
}

TEST(PLA, SetZeroFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;
    nes->cpu->sp = 0xfe;
    nes->cpu->a = 0x00;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::PLA_IMP
    });

    nes->mem->Write(0x01ff, 0x00);

    nes->cpu->Step();
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
}

TEST(PLA, SetNegativeFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;
    nes->cpu->sp = 0xfe;
    nes->cpu->a = 0x00;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::PLA_IMP
    });

    nes->mem->Write(0x01ff, 0x82);

    nes->cpu->Step();
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
}
