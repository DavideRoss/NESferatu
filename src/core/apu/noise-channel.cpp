#include "noise-channel.h"

NoiseChannel::NoiseChannel()
{
    Reset();
}

void NoiseChannel::Reset()
{
    // Envelope
    envelope_loop_ = false;
    constant_volume_ = false;
    envelope_start_ = false;
    volume_ = 0;
    envelope_decay_ = 0;
    envelope_divider_ = 0;

    // TImer and LFSR (linear feedback shift register)
    timer_period_ = PERIOD_TABLE[0];
    timer_counter_ = 0;
    shift_register_ = 1;
    mode_ = false;

    SetHalt(false);
    SetEnabled(false);
}

void NoiseChannel::CPUWrite(u16 address, u8 data)
{
    switch (address)
    {
        // Envelope/volume
        case 0x400c:
        {
            envelope_loop_ = (data & 0x20) != 0;
            constant_volume_ = (data & 0x10) != 0;
            volume_ = data & 0x0f;

            SetHalt(envelope_loop_);
        }; break;

        // Unused register
        case 0x400d: break;

        // Noise period
        case 0x400e:
        {
            mode_ = (data & 0x80) != 0;
            timer_period_ = PERIOD_TABLE[data & 0x0f];
        }; break;

        // Length counter
        case 0x400f:
        {
            if (enabled_)
            {
                LoadIndex(data >> 3);
            }

            envelope_start_ = true;
        }; break;
    }
}

void NoiseChannel::StepTimer()
{
    if (timer_counter_ == 0)
    {
        timer_counter_ = timer_period_ - 1;

        const u16 tap = mode_ ? 6 : 1;
        const u16 feedback = (shift_register_ & 0x01) ^ ((shift_register_ >> tap) & 0x01);

        shift_register_ >>= 1;
        shift_register_ |= feedback << 14;
    }
    else
    {
        --timer_counter_;
    }
}

void NoiseChannel::QuarterFrame()
{
    if (envelope_start_)
    {
        envelope_start_ = false;
        envelope_decay_ = 15;
        envelope_divider_ = volume_;
        return;
    }

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
        else
        {
            envelope_decay_ = 15;
        }
    }
}

void NoiseChannel::SetEnabled(bool enabled)
{
    LengthCounter::SetEnabled(enabled);
}

u8 NoiseChannel::Output() const
{
    if (Counter() == 0 || (shift_register_ & 0x01) != 0)
    {
        return 0;
    }

    return constant_volume_ ? volume_ : envelope_decay_;
}
