#include "dmc-channel.h"

DMCChannel::DMCChannel(Timing timing)
    : timing_(timing)
{
    Reset();
}

void DMCChannel::Reset()
{
    irq_enabled_ = false;
    loop_ = false;
    irq_requested_ = false;

    rate_index_ = 0;
    output_level_ = 0;
    sample_address_ = 0xc000;
    sample_length_ = 1;
    current_address_ = sample_address_;
    bytes_remaining_ = 0;
    sample_buffer_ = 0;
    sample_buffer_full_ = false;

    shift_register_ = 0;
    bits_remaining_ = 8;
    silence_ = true;

    timer_period_ = timing_ == Timing::NTSC ? NTSC_PERIOD_TABLE[rate_index_] : PAL_PERIOD_TABLE[rate_index_];
    timer_ = timer_period_;
}

void DMCChannel::CPUWrite(u16 address, u8 data)
{
    switch (address)
    {
        case 0x4010:
        {
            irq_enabled_ = (data & 0x80) != 0;
            loop_ = (data & 0x40) != 0;
            rate_index_ = data & 0x0f;

            timer_period_ = timing_ == Timing::NTSC ? NTSC_PERIOD_TABLE[rate_index_] : PAL_PERIOD_TABLE[rate_index_];

            if (!irq_enabled_)
            {
                irq_requested_ = false;
            }
        }; break;

        case 0x4011:
        {
            output_level_ = data & 0x7f;
        }; break;

        case 0x4012:
        {
            sample_address_ = 0xc000 + (static_cast<u16>(data) << 6);
        }; break;

        case 0x4013:
        {
            sample_length_ = (static_cast<u16>(data) << 4) | 1u;
        }; break;
    }
}

void DMCChannel::StepTimer()
{
    if (timer_ > 0)
    {
        --timer_;
        return;
    }

    timer_ = timer_period_;
    ClockOutputUnit();
}

bool DMCChannel::SupplySampleByte(u8 data)
{
    if (sample_buffer_full_)
    {
        return false;
    }

    sample_buffer_ = data;
    sample_buffer_full_ = true;
    return true;
}

bool DMCChannel::NeedsSampleFetch() const
{
    return bytes_remaining_ > 0 && !sample_buffer_full_;
}

void DMCChannel::SupplySample(u8 data)
{
    if (sample_buffer_full_ || bytes_remaining_ == 0)
    {
        return;
    }

    sample_buffer_ = data;
    sample_buffer_full_ = true;

    ++current_address_;
    if (current_address_ == 0x0000)
    {
        current_address_ = 0x8000;
    }

    --bytes_remaining_;
    if (bytes_remaining_ == 0)
    {
        if (loop_)
        {
            RestartSample();
        }
        else if (irq_enabled_)
        {
            irq_requested_ = true;
        }
    }
}

void DMCChannel::SetEnabled(bool enabled)
{
    if (!enabled)
    {
        bytes_remaining_ = 0;
    }
    else if (bytes_remaining_ == 0)
    {
        RestartSample();
    }
}

void DMCChannel::ClockOutputUnit()
{
    if (!silence_)
    {
        if ((shift_register_ & 0x01) != 0)
        {
            if (output_level_ <= 125)
            {
                output_level_ += 2;
            }
        }
        else
        {
            if (output_level_ >= 2)
            {
                output_level_ -= 2;
            }
        }
    }

    shift_register_ >>= 1;

    if (bits_remaining_ > 0)
    {
        --bits_remaining_;
    }
    else
    {
        bits_remaining_ = 8;
        if (sample_buffer_full_)
        {
            shift_register_ = sample_buffer_;
            sample_buffer_full_ = false;
            silence_ = false;
        }
        else
        {
            silence_ = true;
        }
    }
}

void DMCChannel::RestartSample()
{
    current_address_ = sample_address_;
    bytes_remaining_ = sample_length_;
}
