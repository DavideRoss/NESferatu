#include <gtest/gtest.h>

#include "test-utils/instruction-fixture.hpp"

TEST(STA, WriteToMemory)
{
    System* nes = new System();

    nes->cpu->pc = 0x000;
    nes->cpu->a = 0x10;

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::STA_ZP, 0x20
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->mem->Read(0x20), 0x10);
}

INSTANTIATE_INSTRUCTION_TEST(
    STA,
    OpcodeTest { Opcodes::STA_ZP, TestAddressingMode::ZeroPage, 3 },
    OpcodeTest { Opcodes::STA_ZPX, TestAddressingMode::ZeroPageX, 4 },
    OpcodeTest { Opcodes::STA_ABS, TestAddressingMode::Absolute, 4 },
    OpcodeTest { Opcodes::STA_ABX, TestAddressingMode::AbsoluteX, 5 },
    OpcodeTest { Opcodes::STA_ABY, TestAddressingMode::AbsoluteY, 5 },
    OpcodeTest { Opcodes::STA_IDX, TestAddressingMode::IndirectX, 6 },
    OpcodeTest { Opcodes::STA_IDY, TestAddressingMode::IndirectY, 6 }
);
