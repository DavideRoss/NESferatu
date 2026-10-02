#pragma once

#include "utils/types.h"

class DMC
{
public:
    void CPUWrite(u16 address, u8 data);

    void Step();

    bool IRQRequested() const { return irq_; }
    bool Active() const { return current_length_ != 0; }

private:
    bool irq_enabled_ = false;
    bool loop_ = false;
    bool irq_ = false;

    u8 rate_index_ = 0;
    u8 output_level_ = 0;

    u16 sample_address_ = 0;
    u16 sample_length = 0;

    u16 current_address_ = 0;
    u16 current_length_ = 0;

    u8 sample_buffer_ = 0;
    bool sample_buffer_empty_ = true;

    u8 shift_register_ = 0;
    u8 bits_remaining_ = 8;

    u16 timer_period_ = 0;
    u16 timer_ = 0;
};
