#include <fstream>

#include "entities/system.h"
#include "entities/cartridge.h"
#include "utils/log.h"

#include "nesferatu-sdl.h"

// 0: normal running mode
// 1: step by instruction
// 2: step by frame
#define MODE_NORMAL 0
#define MODE_INST 1
#define MODE_FRAME 2

#define RUNNING_MODE MODE_FRAME

void SaveFramebuffer(std::span<const Pixel> framebuffer);

int main()
{
    std::cout << "Hello, NESferatu!\n";

    Cartridge cart("roms/Excitebike (Europe).nes");
    // Cartridge cart("roms/tests/color_test.nes");
    System* nes = new System(cart);
    nes->Reset();

    NESFERATU_LOG("Reset PC: 0x{:04x}", nes->cpu->pc);

    NESferatuSDL* sdl = new NESferatuSDL();

    #if RUNNING_MODE == MODE_NORMAL

    using Clock = std::chrono::steady_clock;
    const auto frame_duration = std::chrono::duration<double>(1.0 / 60.0);

    auto next_frame = Clock::now();
    while (sdl->IsRunning())
    {
        sdl->PollEvents();

        while (!nes->IsFrameComplete())
        {
            nes->Step();
        }

        std::span<const Pixel> framebuffer = nes->GetFramebuffer();
        sdl->Render(framebuffer);

        const auto now = Clock::now();
        if (now < next_frame)
        {
            std::this_thread::sleep_until(next_frame);
        }
        else
        {
            next_frame = now;
        }
    }

    #endif

    #if RUNNING_MODE == MODE_INST

    bool run_latch = false;
    bool step_latch = false;

    u32 cycles = 0;
    
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

            if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_l)
            {
                nes->cpu->ToggleLogging();
            }
        }

        if (step_latch || run_latch)
        {
            cycles += nes->Step();
            step_latch = false;

            if (nes->IsFrameComplete())
            {
                NESFERATU_LOG("Frame complete! {}", cycles)
            }

            if (nes->ppu->IsNMIRequested())
            {
                NESFERATU_LOG("Breakpoint hit! Cycles: {}", cycles);
                run_latch = false;
                nes->cpu->ToggleLogging();
            }
        }

        std::span<const Pixel> framebuffer = nes->GetFramebuffer();
        sdl->Render(framebuffer);
    }

    #endif

    #if RUNNING_MODE == MODE_FRAME

    bool run_latch = false;
    bool step_latch = false;
    bool request_screenshot = false;

    u32 cycles = 0;
    u32 frames = 0;
    
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

            if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_l)
            {
                nes->cpu->ToggleLogging();
            }

            if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_p)
            {
                request_screenshot = true;
            }
        }

        if (step_latch || run_latch)
        {
            cycles += nes->StepFrame();
            step_latch = false;
            
            // NESFERATU_LOG("Complete frame {}, cycles: {}", frames, cycles);
            frames++;
        }

        std::span<const Pixel> framebuffer = nes->GetFramebuffer();
        sdl->Render(framebuffer);

        if (request_screenshot)
        {
            SaveFramebuffer(framebuffer);
            request_screenshot = false;
        }

        // TODO: fix quick and dirty vsync
        SDL_Delay(16);
    }

    #endif

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
