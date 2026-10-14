#include "system.h"

System::System(Cartridge& cartridge)
    : cartridge_(cartridge)
{
    cpu = std::make_unique<CPU>();
    mem = std::make_unique<Memory>();
    ppu = std::make_unique<PPU>(cartridge_);
    apu = std::make_unique<APU>();
    controllers = std::make_unique<Controllers>();
    
    bus = std::make_unique<Bus>(
        *cpu,
        *mem,
        *ppu,
        *apu,
        *controllers,
        cartridge_
    );

    debugger = std::make_unique<Debugger>();
    cpu->ConnectDebugger(debugger.get());
    bus->ConnectDebugger(debugger.get());
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

    // TODO: route IRQ/NMI through bus instead?
    if (apu->IsIRQRequested() || cartridge_.IsIRQRequested())
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
