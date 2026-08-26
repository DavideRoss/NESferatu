#include <gtest/gtest.h>

#include "test-utils/utils.hpp"
#include "test-utils/instruction-fixture.hpp"

TEST(STY, WriteToMemory)
{
    System* nes = TestUtils::SetupSystem();

    nes->cpu->pc = 0x000;
    nes->cpu->y = 0x10;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::STY_ZP, 0x20
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->mem->Read(0x20), 0x10);
}

INSTANTIATE_INSTRUCTION_TEST(
    STY,
    OpcodeTest { Opcodes::STY_ZP, TestAddressingMode::ZeroPage, 3 },
    OpcodeTest { Opcodes::STY_ZPX, TestAddressingMode::ZeroPageX, 4 },
    OpcodeTest { Opcodes::STY_ABS, TestAddressingMode::Absolute, 4 }
);
