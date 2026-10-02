#pragma once

#include "length-counter.h"

class NoiseChannel : public LengthCounter
{
public:
    void CPUWrite(u16 address, u8 data);

    void QuarterFrame();

private:
    bool control_volume_ = false;
    u8 volume_ = 0;

    u8 period_index_ = 0;
    bool mode_ = false;

    u16 timer_period_ = 0;
    u16 timer_ = 0;

    u16 shift_register_ = 1;
};
