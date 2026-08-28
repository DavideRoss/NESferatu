#include <gtest/gtest.h>

#include "test-utils/instruction-fixture.hpp"

TEST(SBC, BasicSubtraction)
{
    System* nes = new System();
    nes->cpu->pc = 0x000;

    nes->cpu->a = 0x50;
    nes->cpu->SetFlag(StatusFlag::C, true);

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::SBC_IM, 0x20
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x30);
}

TEST(SBC, WithBorrow)
{
    System* nes = new System();
    nes->cpu->pc = 0x000;

    nes->cpu->a = 0x50;
    nes->cpu->SetFlag(StatusFlag::C, false);

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::SBC_IM, 0x20
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0x2f);
}

TEST(SBC, NoBorrowSetCarryFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x000;

    nes->cpu->a = 0x50;
    nes->cpu->SetFlag(StatusFlag::C, true);

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::SBC_IM, 0x20
    });

    nes->cpu->Step();
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::C));
}

TEST(SBC, BorrowClearCarryFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x000;

    nes->cpu->a = 0x20;
    nes->cpu->SetFlag(StatusFlag::C, true);

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::SBC_IM, 0x30
    });

    nes->cpu->Step();
    EXPECT_EQ(nes->cpu->a, 0xf0);
    EXPECT_FALSE(nes->cpu->GetFlag(StatusFlag::C));
}

TEST(SBC, SetZeroFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x000;

    nes->cpu->a = 0x42;
    nes->cpu->SetFlag(StatusFlag::C, true);

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::SBC_IM, 0x42
    });

    nes->cpu->Step();
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::Z));
}

TEST(SBC, SetNegativeFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x000;

    nes->cpu->a = 0x20;
    nes->cpu->SetFlag(StatusFlag::C, true);

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::SBC_IM, 0x30
    });

    nes->cpu->Step();
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::N));
}

TEST(SBC, SetOverflowFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x000;

    nes->cpu->a = 0x7f;
    nes->cpu->SetFlag(StatusFlag::C, true);

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::SBC_IM, 0xff
    });

    nes->cpu->Step();
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::V));
}

TEST(SBC, SetNegativeOverflowFlag)
{
    System* nes = new System();
    nes->cpu->pc = 0x000;

    nes->cpu->a = 0x80;
    nes->cpu->SetFlag(StatusFlag::C, true);

    nes->mem->WriteSeq(0x0000,
    {
        Opcodes::SBC_IM, 0x01
    });

    nes->cpu->Step();
    EXPECT_TRUE(nes->cpu->GetFlag(StatusFlag::V));
}

INSTANTIATE_INSTRUCTION_TEST(
    SBC,
    OpcodeTest { Opcodes::SBC_IM, TestAddressingMode::Immediate, 2 },
    OpcodeTest { Opcodes::SBC_ZP, TestAddressingMode::ZeroPage, 3 },
    OpcodeTest { Opcodes::SBC_ZPX, TestAddressingMode::ZeroPageX, 4 },
    OpcodeTest { Opcodes::SBC_ABS, TestAddressingMode::Absolute, 4 },
    OpcodeTest { Opcodes::SBC_ABX, TestAddressingMode::AbsoluteX, 4 },
    OpcodeTest { Opcodes::SBC_ABX, TestAddressingMode::AbsoluteXPageCrossing, 5 },
    OpcodeTest { Opcodes::SBC_ABY, TestAddressingMode::AbsoluteY, 4 },
    OpcodeTest { Opcodes::SBC_ABY, TestAddressingMode::AbsoluteYPageCrossing, 5 },
    OpcodeTest { Opcodes::SBC_IDX, TestAddressingMode::IndirectX, 6 },
    OpcodeTest { Opcodes::SBC_IDY, TestAddressingMode::IndirectY, 5 },
    OpcodeTest { Opcodes::SBC_IDY, TestAddressingMode::IndirectYPageCrossing, 6 }
);
