#include "length-counter.h"

#include <array>

namespace
{
    static constexpr std::array<u8, 32> LENGTH_TABLE = {
        10, 254,  20,   2,
        40,   4,  80,   6,
        160,   8,  60,  10,
        14,  12,  26,  14,
        12,  16,  24,  18,
        48,  20,  96,  22,
        192,  24,  72,  26,
        16,  28,  32,  30
    };
}

void LengthCounter::SetEnabled(bool enabled)
{
    enabled_ = enabled;
    if (!enabled_)
    {
        counter_ = 0;
    }
}

void LengthCounter::HalfFrame()
{
    if (counter_ != 0 && !halt_)
    {
        --counter_;
    }
}

void LengthCounter::LoadIndex(u8 index)
{
    if (enabled_)
    {
        counter_ = LENGTH_TABLE[index & 0x1f];
    }
}
