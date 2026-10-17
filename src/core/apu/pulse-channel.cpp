#include "pulse-channel.h"

#include <algorithm>
#include <array>

namespace
{
    constexpr std::array<std::array<u8, 8>, 4> DUTY_SEQUENCES =
    {{
        {{ 0, 1, 0, 0, 0, 0, 0, 0 }},
        {{ 0, 1, 1, 0, 0, 0, 0, 0 }},
        {{ 0, 1, 1, 1, 1, 0, 0, 0 }},
        {{ 1, 0, 0, 1, 1, 1, 1, 1 }}
    }};
}

void PulseChannel::Reset()
{
    duty_ = 0;
    constant_volume_ = false;
    volume_ = 0;

    sweep_enabled_ = false;
    sweep_period_ = 0;
    sweep_negate_ = false;
    sweep_shift_ = 0;
    sweep_reload_ = false;
    sweep_divider_ = 0;

    envelope_loop_ = false;
    envelope_start_ = false;

    envelope_divider_ = 0;
    envelope_decay_ = 0;

    timer_period_ = 0;
    timer_ = 0;
    seq_step_ = 0;
}

void PulseChannel::QuarterFrame()
{
    if (envelope_start_)
    {
        envelope_start_ = false;
        envelope_decay_ = 15;
        envelope_divider_ = volume_;
    }
    else
    {
        if (envelope_divider_ > 0)
        {
            --envelope_divider_;
        }
        else
        {
            envelope_divider_ = volume_;

            if (envelope_decay_ > 0)
            {
                --envelope_decay_;
            }
            else if (envelope_loop_)
            {
                envelope_decay_ = 15;
            }
        }
    }
}

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

            timer_ = timer_period_;
            seq_step_ = 0;
            envelope_start_ = true;
        }; break;
    }
}

bool PulseChannel::SweepMuted() const
{
    u16 target;
    return timer_period_ < 8 || !SweepTarget(target);
}

u8 PulseChannel::Output() const
{
    if (Counter() == 0 || SweepMuted()) return 0;

    if (DUTY_SEQUENCES[duty_][seq_step_] == 0) return 0;

    return constant_volume_ ? volume_ : envelope_decay_;
}

void PulseChannel::ClockSweep()
{
    const bool divider_expired = sweep_divider_ == 0;
    if (divider_expired && sweep_enabled_ && sweep_shift_ != 0 && !SweepMuted())
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
    const i32 change = timer_period_ >> sweep_shift_;

    if (sweep_negate_)
    {
        const i32 result =
            static_cast<i32>(timer_period_) -
            change -
            (is_pulse1_ ? 1 : 0);

        target = static_cast<u16>(std::max<i32>(0, result));
        return true;
    }

    const u32 result = static_cast<u32>(timer_period_) + change;

    if (result > 0x07ff)
    {
        return false;
    }

    target = static_cast<u16>(result);
    return true;
}

void PulseChannel::StepTimer()
{
    if (timer_ == 0)
    {
        timer_ = timer_period_;
        seq_step_ = (seq_step_ + 1) & 0x07;
    }
    else
    {
        --timer_;
    }
}
