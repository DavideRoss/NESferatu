#pragma once

#include "apu.h"
#include "cartridge.h"
#include "controllers.h"
#include "cpu.h"
#include "ppu.h"

#include "utils/defines.h"
#include "utils/types.h"

enum class OAMDMAPhase
{
    Dummy,
    Alignment,
    Read,
    Write
};

class Bus NESFERATU_DEBUGGABLE
{

public:
    Bus(
        CPU& cpu,
        PPU& ppu,
        APU& apu,
        Controllers& controllers
    );

    void Reset();

    void SlotCartridge(std::shared_ptr<Cartridge> cartridge);
    bool IsCartridgeSlotted() const { return cartridge_ != nullptr; }

    u8 CPURead(u16 address);
    void CPUWrite(u16 address, u8 data);

    u8 PPURead(u16 address);
    void PPUWrite(u16 address, u8 data);

    bool IsOAMDMAActive();
    void StepOAMDMA();

    Mirroring GetCartridgeMirroring() const;
    bool IsCartridgeIRQRequested() const;

    void Serialize(class Serializer& serializer);
    void Deserialize(class Deserializer& deserializer);

private:
    CPU& cpu_;
    PPU& ppu_;
    APU& apu_;
    Controllers& controllers_;
    
    std::shared_ptr<Cartridge> cartridge_ = nullptr;

    // OAM DMA
    bool oam_dma_active_ = false;
    OAMDMAPhase oam_dma_phase_;
    u8 oam_dma_page_;
    u8 oam_dma_address_;
    u8 oam_dma_data_;
};
