#pragma once

#include "apu.h"
#include "cartridge.h"
#include "controllers.h"
#include "cpu.h"
#include "memory.h"
#include "ppu.h"

#include "debugger/debuggable.h"

#include "utils/types.h"

enum class OAMDMAPhase
{
    Dummy,
    Alignment,
    Read,
    Write
};

class Bus : public Debuggable
{

public:
    Bus(
        CPU& cpu,
        Memory& memory,
        PPU& ppu,
        APU& apu,
        Controllers& controllers,
        Cartridge& cartridge
    );

    u8 CPURead(u16 address);
    void CPUWrite(u16 address, u8 data);

    u8 PPURead(u16 address);
    void PPUWrite(u16 address, u8 data);

    bool IsOAMDMAActive();
    void StepOAMDMA();

private:
    CPU& cpu_;
    Memory& memory_;
    PPU& ppu_;
    APU& apu_;
    Controllers& controllers_;
    Cartridge& cartridge_;

    // OAM DMA
    bool oam_dma_active_;
    OAMDMAPhase oam_dma_phase_;
    u8 oam_dma_page_;
    u8 oam_dma_address_;
    u8 oam_dma_data_;
};
