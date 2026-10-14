#pragma once

#include <array>

#include "length-counter.h"

class TriangleChannel : public LengthCounter
{
public:
    void Reset();

    void CPUWrite(u16 address, u8 data);

    void StepTimer();
    void QuarterFrame();

    void SetEnabled(bool enabled);

    u8 Output() const;

private:
    bool ctrl_flag_ = false;

    u8 linear_counter_reload_ = 0;
    u8 linear_counter_ = 0;
    bool linear_reload_ = false;

    u16 timer_period_ = 0;
    u16 timer_ = 0;

    u8 sequence_step_ = 0;

private:
    static constexpr std::array<u8, 32> SEQUENCE = {
        15, 14, 13, 12, 11, 10,  9,  8,
         7,  6,  5,  4,  3,  2,  1,  0,
         0,  1,  2,  3,  4,  5,  6,  7,
         8,  9, 10, 11, 12, 13, 14, 15
    };
};
