#include <gtest/gtest.h>

#include "entities/memory.h"

TEST(Memory, ReadOutOfBoundsAddress)
{
    EXPECT_DEATH([]{
        Memory* mem = new Memory();
        u8 data = mem->Read(0x1fff);
    }(), "");
}

TEST(Memory, WriteOutOfBoundsAddress)
{
    EXPECT_DEATH([]{
        Memory* mem = new Memory();
        mem->Write(0x1fff, 0x80);
    }(), "");
}

TEST(Memory, WriteReadByte)
{
    Memory* mem = new Memory();
    mem->Write(0x0123, 0x45);
    u8 data = mem->Read(0x0123);

    EXPECT_EQ(data, 0x45);
}

TEST(Memory, WriteSequence)
{
    Memory* mem = new Memory();

    mem->WriteSeq(0x0000, {
        0x01, 0x02,
        0x03, 0x04,
    });

    EXPECT_EQ(mem->Read(0x000), 0x01);
    EXPECT_EQ(mem->Read(0x001), 0x02);
    EXPECT_EQ(mem->Read(0x002), 0x03);
    EXPECT_EQ(mem->Read(0x003), 0x04);
}