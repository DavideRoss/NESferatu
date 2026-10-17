#include "system.h"

System::System()
{
    cpu = std::make_unique<CPU>();
    mem = std::make_unique<Memory>();
    ppu = std::make_unique<PPU>();
    apu = std::make_unique<APU>();
    controllers = std::make_unique<Controllers>();
    
    bus = std::make_unique<Bus>(
        *cpu,
        *mem,
        *ppu,
        *apu,
        *controllers
    );

    debugger = std::make_unique<Debugger>();
    cpu->ConnectDebugger(debugger.get());
    bus->ConnectDebugger(debugger.get());

    Reset();
}

void System::SlotCartridge(std::shared_ptr<Cartridge> cartridge)
{
    bus->SlotCartridge(cartridge);
    Reset();
}

bool System::IsCartridgeSlotted() const
{
    return bus->IsCartridgeSlotted();
}

void System::Reset()
{
    cpu->Reset();
    ppu->Reset();
    apu->Reset();
    controllers->Reset();
}

u32 System::Step()
{
    if (!bus->IsCartridgeSlotted())
    {
        NESFERATU_THROW("Cartridge is not slotted");
    }

    if (bus->IsOAMDMAActive())
    {
        bus->StepOAMDMA();
        StepHardware();
        ++cpu->total_cycles;
        return 1;
    }

    if (apu->DMCNeedsSampleFetch())
    {
        const u16 address = apu->GetDMCReadAddress();
        const u8 data = bus->CPURead(address);

        apu->SupplyDMCByte(data);

        constexpr u32 DMC_DMA_CYCLES = 4;

        for (u32 i = 0; i < DMC_DMA_CYCLES; ++i)
        {
            StepHardware();
        }

        cpu->total_cycles += DMC_DMA_CYCLES;
        return DMC_DMA_CYCLES;
    }

    const u32 cycles = cpu->Step();

    for (u32 i = 0; i < cycles; ++i)
    {
        StepHardware();
    }

    return cycles;
}

void System::StepHardware()
{
    apu->Step();
    apu->GenerateAudioSample();

    for (u32 i = 0; i < 3; ++i)
    {
        ppu->Step();
        if (ppu->IsNMIRequested())
        {
            cpu->RequestNMI();
            ppu->ClearNMIRequest();
        }
    }

    if (apu->IsIRQRequested() || bus->IsCartridgeIRQRequested())
    {
        cpu->RequestIRQ();
    }
}

u32 System::StepFrame()
{
    u32 cycles = 0;
    while (!ppu->IsFrameComplete())
    {
        cycles += Step();
    }

    ppu->ClearFrameComplete();
    return cycles;
}

bool System::IsFrameComplete() const
{
    return ppu->IsFrameComplete();
}

std::span<const Pixel> System::GetFramebuffer()
{
    return ppu->GetFramebuffer();
}

void System::ClearFrameComplete()
{
    ppu->ClearFrameComplete();
}
