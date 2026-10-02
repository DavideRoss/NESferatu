#include "pulse-channel.h"

void PulseChannel::HalfFrame()
{
    LengthCounter::HalfFrame();
    
    ClockSweep();
}

void PulseChannel::CPUWrite(u16 address, u8 data)
{
    switch (address)
    {
        // Duty/envelope
        case 0x4000:
        {
            duty_ = (data >> 6) & 0x03;
            envelope_loop_ = data & 0x20;
            constant_volume_ = data & 0x10;
            volume_ = data & 0x0f;

            SetHalt(envelope_loop_);
        }; break;

        // Sweep
        case 0x4001:
        {
            sweep_enabled_ = data & 0x80;
            sweep_period_ = (data >> 4) & 0x07;
            sweep_negate_ = data & 0x08;
            sweep_shift_ = data & 0x07;
            
            sweep_reload_ = true;
        }; break;

        // Timer low
        case 0x4002:
        {
            timer_period_ = (timer_period_ & 0x0700) | data;
        }; break;

        // Length/timer high
        case 0x4003:
        {
            timer_period_ = (timer_period_ & 0x00ff) | ((data & 0x07) << 8);
            LoadIndex(data >> 3);

            seq_step_ = 0;

            // TODO: restart envelope
        }; break;
    }
}

void PulseChannel::ClockSweep()
{
    const bool divider_expired = sweep_divider_ == 0;
    if (divider_expired && sweep_enabled_ && sweep_shift_ != 0)
    {
        u16 target;
        if (SweepTarget(target))
        {
            timer_period_ = target;
        }
    }

    if (divider_expired || sweep_reload_)
    {
        sweep_divider_ = sweep_period_;
        sweep_reload_ = false;
    }
    else
    {
        --sweep_divider_;
    }
}

bool PulseChannel::SweepTarget(u16 &target) const
{
    const u16 change = timer_period_ >> sweep_shift_;

    if (sweep_negate_)
    {
        const u16 negate = is_pulse1_ ? 1 : 0;
        if (timer_period_ < change + negate)
        {
            return false;
        }

        target = timer_period_ - change - negate;
        return true;
    }

    const u32 result = static_cast<u32>(timer_period_) + change;

    if (result > 0x07ff)
    {
        return false;
    }

    target = result;
    return true;
}
