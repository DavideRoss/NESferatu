#pragma once

#include "apu.h"
#include "bus.h"
#include "controllers.h"
#include "cpu.h"
#include "ppu.h"

#ifdef NESFERATU_DEBUG
#include "debugger/debugger.h"
#endif

class System
{

public:
    System();

    void Reset();

    void SlotCartridge(std::shared_ptr<Cartridge> cartridge);
    bool IsCartridgeSlotted() const;

    u32 Step();
    u32 StepFrame();

    std::span<const Pixel> GetFramebuffer();
    bool IsFrameComplete() const;
    void ClearFrameComplete();
    
    std::vector<std::byte> WriteSavestate();
    void ReadSavestate(std::span<const std::byte> data);

private:
    void StepHardware();

public:
    std::unique_ptr<Bus> bus;
    std::unique_ptr<CPU> cpu;
    std::unique_ptr<PPU> ppu;
    std::unique_ptr<APU> apu;
    std::unique_ptr<Controllers> controllers;

    #ifdef NESFERATU_DEBUG
    std::unique_ptr<Debugger> debugger;
    #endif
};
