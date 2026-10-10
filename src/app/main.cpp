#include <csignal>
#include <fstream>

#include "debugger/debugger.h"
#include "entities/system.h"
#include "entities/cartridge.h"
#include "utils/log.h"

#include "nesferatu-sdl.h"

void SaveFramebuffer(std::span<const Pixel> framebuffer);
void DebuggerBreakCallback(const DebugContext& context);

int main()
{
    Cartridge cart("roms/Tetris (Europe).nes");
    std::unique_ptr<System> nes = std::make_unique<System>(cart);
    nes->Reset();

    std::unique_ptr<NESferatuSDL> sdl = std::make_unique<NESferatuSDL>();

    bool run_latch = true;
    bool step_latch = false;
    bool request_screenshot = false;

    nes->debugger->SetBreakCallback(DebuggerBreakCallback);

    // nes->debugger->AddBreakpoint({
    //     .event = DebugEventType::CPUExecute,
    //     .pc = 0x80df,
    // });
    
    bool running = true;
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }

            if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_s)
            {
                step_latch = true;
            }

            if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_r)
            {
                run_latch = !run_latch;
            }

            if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_p)
            {
                request_screenshot = true;
            }

            if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_1)
            {
                nes->ppu->SetDebugViewMode(DebugViewMode::Palette);
            }

            if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_2)
            {
                nes->ppu->SetDebugViewMode(DebugViewMode::FineX);
            }

            if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_3)
            {
                nes->ppu->SetDebugViewMode(DebugViewMode::PixelPresence);
            }

            if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_4)
            {
                nes->ppu->SetDebugViewMode(DebugViewMode::Nametable);
            }

            if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_0)
            {
                nes->ppu->SetDebugViewMode(DebugViewMode::None);
            }
        }

        const Uint8* keyboard = SDL_GetKeyboardState(nullptr);
        u8 status = 0;

        if (keyboard[SDL_SCANCODE_Z])           status |= static_cast<u8>(ControllerButtons::ButtonA);
        if (keyboard[SDL_SCANCODE_X])           status |= static_cast<u8>(ControllerButtons::ButtonB);
        if (keyboard[SDL_SCANCODE_RETURN])      status |= static_cast<u8>(ControllerButtons::ButtonStart);
        if (keyboard[SDL_SCANCODE_BACKSPACE])   status |= static_cast<u8>(ControllerButtons::ButtonSelect);
        if (keyboard[SDL_SCANCODE_UP])          status |= static_cast<u8>(ControllerButtons::ButtonUp);
        if (keyboard[SDL_SCANCODE_DOWN])        status |= static_cast<u8>(ControllerButtons::ButtonDown);
        if (keyboard[SDL_SCANCODE_LEFT])        status |= static_cast<u8>(ControllerButtons::ButtonLeft);
        if (keyboard[SDL_SCANCODE_RIGHT])       status |= static_cast<u8>(ControllerButtons::ButtonRight);

        nes->controllers->SetController1Status(status);

        while (!nes->IsFrameComplete() && (step_latch || run_latch))
        {
            if (step_latch || run_latch)
            {
                nes->Step();
                step_latch = false;
            }
        }

        sdl->Clear();

        std::span<const Pixel> framebuffer = nes->GetFramebuffer();
        sdl->Render(framebuffer);

        if (request_screenshot)
        {
            SaveFramebuffer(framebuffer);
            request_screenshot = false;
        }

        nes->ClearFrameComplete();

        // Draw palette
        std::array<Pixel, 32> palette = nes->ppu->DumpPalette();
        for (u8 y = 0; y < 2; ++y)
        {
            for (u8 x = 0; x < 16; ++x)
            {
                u8 index = y * 16 + x;
                sdl->DrawRectangle(palette[index], 32 * x, 32 * y, 32, 32);
            }
        }
        

        sdl->Present();

        // TODO: write proper timing
        SDL_Delay(4);
    }

    return 0;
}

void SaveFramebuffer(std::span<const Pixel> framebuffer)
{
    const auto now = std::chrono::system_clock::now();
    const time_t time = std::chrono::system_clock::to_time_t(now);

    std::ostringstream name;
    name << "temp/"
        << std::put_time(std::localtime(&time), "%Y-%m-%d_%H-%M-%S")
        << ".ppm";

    std::ofstream file(name.str(), std::ios::binary);

    file << "P6\n256 240\n255\n";
    file.write(reinterpret_cast<const char*>(framebuffer.data()), framebuffer.size_bytes());

    NESFERATU_LOG("File saved: {}", name.str());
}

void DebuggerBreakCallback(const DebugContext& context)
{
    std::raise(SIGTRAP);
}
