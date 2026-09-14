#pragma once

#include "utils/types.h"

class Bus;

class APU
{
public:
    void ConnectBus(Bus* bus);
    void Reset();

    u8 CPURead(u16 address);
    void CPUWrite(u16 address, u8 val);

private:
    u8 pulse1_timer_;
    u8 pulse1_length_;
    u8 pulse1_envelope_;
    u8 pulse1_sweep_;

    u8 pulse2_timer_;
    u8 pulse2_length_;
    u8 pulse2_envelope_;
    u8 pulse2_sweep_;

    u8 tri_timer_;
    u8 tri_length_;
    u8 tri_counter_;

    u8 noise_timer_;
    u8 noise_length_;
    u8 noise_envelope_;
    u8 noise_feedback_;

    u8 dmc_timer_;
    u8 dmc_mem_reader_;
    u8 dmc_buffer_;
    u8 dmc_output_;

    u8 status_;
    u8 frame_counter_;

private:
    Bus* bus_ = nullptr;
};
