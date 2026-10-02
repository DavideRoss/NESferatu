#pragma once

#include "length-counter.h"

class PulseChannel : public LengthCounter
{
public:
    explicit PulseChannel(bool is_pulse1)
        : is_pulse1_(is_pulse1)
    {}

    void QuarterFrame();
    void HalfFrame() override;

    void CPUWrite(u16 address, u8 data);

private:
    void ClockSweep();
    bool SweepTarget(u16& target) const;

private:
    bool is_pulse1_;

    u8 duty_ = 0;
    bool constant_volume_ = false;
    u8 volume_ = 0;

    bool sweep_enabled_ = false;
    u8 sweep_period_ = 0;
    bool sweep_negate_ = false;
    u8 sweep_shift_ = 0;
    bool sweep_reload_ = false;
    u8 sweep_divider_ = 0;

    bool envelope_loop_ = false;

    u16 timer_period_ = 0;
    u16 timer_ = 0;

    u8 seq_step_ = 0;
};
