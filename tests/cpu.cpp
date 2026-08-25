#include <gtest/gtest.h>

#include "entities/cpu.h"

TEST(CPU, Reset)
{
    CPU* cpu = new CPU();
    cpu->Reset();

    EXPECT_EQ(cpu->a, 0x0);
    EXPECT_EQ(cpu->x, 0x0);
    EXPECT_EQ(cpu->y, 0x0);
    EXPECT_EQ(cpu->pc, 0xfffc);
    EXPECT_EQ(cpu->sp, 0xfd);
    EXPECT_EQ(cpu->s, 0x00);
}

TEST(CPU, SetFlag)
{
    CPU* cpu = new CPU();
    cpu->Reset();

    cpu->SetFlag(StatusFlag::Z, true);
    EXPECT_EQ(cpu->s, 0x02);
}