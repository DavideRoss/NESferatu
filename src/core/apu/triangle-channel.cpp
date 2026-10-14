#include "triangle-channel.h"

void TriangleChannel::Reset()
{
    timer_period_ = 0;
    timer_ = 0;
    sequence_step_ = 0;

    linear_counter_reload_ = 0;
    linear_counter_ = 0;

    ctrl_flag_ = false;
    linear_reload_ = false;

    SetHalt(false);
    LoadIndex(0);
    SetEnabled(false);
}

void TriangleChannel::CPUWrite(u16 address, u8 data)
{
    switch (address)
    {
        // Length counter
        case 0x4008:
        {
            ctrl_flag_ = (data & 0x80) != 0;
            linear_counter_reload_ = data & 0x7f;
            SetHalt(ctrl_flag_);
        }; break;

        // Timer low
        case 0x400a:
        {
            timer_period_ = (timer_period_ & 0x0700) | data;
        }; break;

        // Length/timer high
        case 0x400b:
        {
            timer_period_ = (timer_period_ & 0x00ff) | ((data & 0x07) << 8);

            if (enabled_)
            {
                LoadIndex(data >> 3);
            }

            linear_reload_ = true;
        }; break;
    }
}

void TriangleChannel::StepTimer()
{
    if (timer_ == 0)
    {
        timer_ = timer_period_;

        if (timer_period_ >= 2 && Counter() > 0 && linear_counter_ > 0)
        {
            sequence_step_ = (sequence_step_ + 1) & 0x1f;
        }
    }
    else
    {
        --timer_;
    }
}

void TriangleChannel::QuarterFrame()
{
    if (linear_reload_)
    {
        linear_counter_ = linear_counter_reload_;
    }
    else if (linear_counter_ > 0)
    {
        --linear_counter_;
    }

    if (!ctrl_flag_)
    {
        linear_reload_ = false;
    }
}

// TODO: can I make the length counter function virtual and not explicitly write this method?
void TriangleChannel::SetEnabled(bool enabled)
{
    LengthCounter::SetEnabled(enabled);
}

u8 TriangleChannel::Output() const
{
    return SEQUENCE[sequence_step_];
}
