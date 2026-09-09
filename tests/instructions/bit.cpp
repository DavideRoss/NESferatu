#include <gtest/gtest.h>

#include "test-utils/utils.hpp"
#include "test-utils/instruction-fixture.hpp"

TEST(BIT, SetZeroFlag)
{
    System* nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;

    nes->cpu->a = 0x0f;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::BIT_ZP, 0x10
    });
    nes->mem->Write(0x0010, 0xf0);

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x0f);
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
}

TEST(BIT, SetNegativeFlag)
{
    System *nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;

    nes->mem->WriteSeq(0x0000, {
        Opcodes::BIT_ZP, 0x10
    });
    nes->mem->Write(0x0010, 0x80);
    
    nes->cpu->Step();
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
}

TEST(BIT, SetOverflowFlag)
{
    System *nes = TestUtils::CreateTestSystem();
    nes->cpu->pc = 0x0000;
    nes->cpu->SetFlag(StatusFlag::Z, true);

    nes->mem->WriteSeq(0x0000, {
        Opcodes::BIT_ZP, 0x10
    });
    nes->mem->Write(0x0010, 0x40);
    
    nes->cpu->Step();
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::V));
}

INSTANTIATE_INSTRUCTION_TEST(
    BIT,
    OpcodeTest { Opcodes::BIT_ZP, TestAddressingMode::ZeroPage, 3 },
    OpcodeTest { Opcodes::BIT_ABS, TestAddressingMode::Absolute, 4 }
);
