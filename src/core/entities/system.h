#pragma once

#include "apu.h"
#include "bus.h"
#include "controllers.h"
#include "cpu.h"
#include "memory.h"
#include "ppu.h"

#include "debugger/debugger.h"

class System
{

public:
    System();

    void Reset();

    void SlotCartridge(std::shared_ptr<Cartridge> cartridge);
    bool IsCartridgeSlotted() const;

    u32 Step();
    u32 StepFrame();

    bool IsFrameComplete() const;
    std::span<const Pixel> GetFramebuffer();
    void ClearFrameComplete();

    std::vector<std::byte> WriteSavestate();
    void ReadSavestate(std::span<const std::byte> data);

private:
    void StepHardware();

public:
    std::unique_ptr<Bus> bus;
    std::unique_ptr<CPU> cpu;
    std::unique_ptr<Memory> mem;
    std::unique_ptr<PPU> ppu;
    std::unique_ptr<APU> apu;
    std::unique_ptr<Controllers> controllers;

    std::unique_ptr<Debugger> debugger;
};
