#pragma once

#include "length-counter.h"

class TriangleChannel : public LengthCounter
{
public:
    void CPUWrite(u16 address, u8 data);

    void QuarterFrame();

private:
    bool ctrl_flag_ = false;

    u8 linear_counter_reload_ = 0;
    u8 linear_counter_ = 0;
    bool linear_reload_ = false;

    u16 timer_period_ = 0;
    u16 timer_ = 0;

    u8 sequence_step_ = 0;
};
