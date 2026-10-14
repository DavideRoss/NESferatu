#pragma once

#include <array>

#include "utils/types.h"

// Delta modulation channel
class DMCChannel
{
public:
    enum class Timing
    {
        NTSC,
        PAL
    };

public:
    explicit DMCChannel(Timing timing = Timing::NTSC);

    void Reset();

    void CPUWrite(u16 address, u8 data);

    void StepTimer();

    bool SupplySampleByte(u8 data);
    bool NeedsSampleFetch() const;
    void SupplySample(u8 data);

    u8 Output() const { return output_level_; }

    u16 GetCurrentAddress() const { return current_address_; }
    u16 BytesRemaining() const { return bytes_remaining_; }

    void SetEnabled(bool enabled);

    bool IsActive() const { return bytes_remaining_ > 0; }
    bool IsLooping() const { return loop_; }
    bool IsSampleBufferEmpty() const { return !sample_buffer_full_; }
    bool IsIRQEnabled() const { return irq_enabled_; }
    bool IsIRQRequested() const { return irq_requested_; }
    void ClearIRQRequest() { irq_requested_ = false; }

private:
    void ClockOutputUnit();
    void RestartSample();

private:
    static constexpr std::array<u16, 16> NTSC_PERIOD_TABLE = {
        428, 380, 340, 320, 286, 254, 226, 214,
        190, 160, 142, 128, 106,  85,  72,  54
    };

    static constexpr std::array<u16, 16> PAL_PERIOD_TABLE = {
        398, 354, 316, 298, 276, 236, 210, 198,
        176, 148, 132, 118,  98,  78,  66,  50
    };

private:
    Timing timing_;

    bool irq_enabled_ = false;
    bool loop_ = false;
    bool irq_requested_ = false;

    u8 rate_index_ = 0;
    u8 output_level_ = 0;
    u16 sample_address_ = 0xc000;
    u16 sample_length_ = 1;
    u16 current_address_ = 0xc000;
    u16 bytes_remaining_ = 0;
    u8 sample_buffer_ = 0;
    bool sample_buffer_full_ = false;

    u8 shift_register_ = 0;
    u8 bits_remaining_ = 8;
    bool silence_ = true;

    u16 timer_period_ = 428;
    u16 timer_ = 428;
};
