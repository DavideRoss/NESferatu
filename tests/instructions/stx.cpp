#include <gtest/gtest.h>

#include "test-utils/utils.hpp"
#include "test-utils/instruction-fixture.hpp"

TEST(STX, WriteToMemory)
{
    System* nes = TestUtils::CreateTestSystem();

    nes->cpu->pc = 0x0000;
    nes->cpu->x = 0x10;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::STX_ZP, 0x20
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->mem->Read(0x20), 0x10);
}

INSTANTIATE_INSTRUCTION_TEST(
    STX,
    OpcodeTest { Opcodes::STX_ZP, TestAddressingMode::ZeroPage, 3 },
    OpcodeTest { Opcodes::STX_ZPY, TestAddressingMode::ZeroPageY, 4 },
    OpcodeTest { Opcodes::STX_ABS, TestAddressingMode::Absolute, 4 }
);
