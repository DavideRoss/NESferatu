#include <gtest/gtest.h>

#include "test-utils/utils.hpp"

TEST(BRK, ForceInterrupt)
{
    Cartridge cart("../../tests/test-roms/reset-vector-test.nes");
    System* nes = new System(cart);
    nes->Reset();

    nes->cpu->pc = 0x0123;
    nes->cpu->SetFlag(StatusFlag::N, true);
    nes->cpu->SetFlag(StatusFlag::Z, true);

    nes->mem->Write(0x0123, Opcodes::BRK_IMP);

    const u32 cycles = nes->cpu->Step();
    EXPECT_EQ(cycles, 7);
    EXPECT_EQ(nes->cpu->pc, 0xabcd);
    EXPECT_EQ(nes->cpu->sp, 0xfc);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::I));

    // Check stack
    EXPECT_EQ(nes->mem->Read(0x01ff), 0x01);
    EXPECT_EQ(nes->mem->Read(0x01fe), 0x25);
    EXPECT_EQ(nes->mem->Read(0x01fd), 0xa2);
}
