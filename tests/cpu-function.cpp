#include <gtest/gtest.h>

#include "entities/cpu.h"

TEST(BasicTest, AdditionWorks)
{
    EXPECT_EQ(2 + 2, 4);
}

TEST(CPUFunctions, Reset)
{
    CPU* cpu = new CPU();
    cpu->Reset();

    EXPECT_EQ(cpu->a, 0x0);
    EXPECT_EQ(cpu->x, 0x0);
    EXPECT_EQ(cpu->y, 0x0);
    EXPECT_EQ(cpu->pc, 0xfffc);
    EXPECT_EQ(cpu->s, 0x0);
    EXPECT_EQ(cpu->p, 0x36);
}
