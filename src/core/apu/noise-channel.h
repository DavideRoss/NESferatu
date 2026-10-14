#pragma once

#include <array>

#include "length-counter.h"

class NoiseChannel : public LengthCounter
{
public:
    NoiseChannel();

    void Reset();

    void CPUWrite(u16 address, u8 data);

    void StepTimer();
    void QuarterFrame();

    void SetEnabled(bool enabled);

    u8 Output() const;

private:
    static constexpr std::array<u16, 16> PERIOD_TABLE = {
        4, 8, 16, 32,
        64, 96, 128, 160,
        202, 254, 380, 508,
        762, 1016, 2034, 4068
    };

private:
    // Envelope
    bool envelope_loop_ = false;
    bool constant_volume_ = false;
    bool envelope_start_ = false;
    u8 volume_ = 0;
    u8 envelope_decay_ = 0;
    u8 envelope_divider_ = 0;

    // TImer and LFSR (linear feedback shift register)
    u16 timer_period_ = 4;
    u16 timer_counter_ = 0;
    u16 shift_register_ = 1;
    bool mode_ = false;
};
