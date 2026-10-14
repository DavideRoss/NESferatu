#include "app.h"

#include <format>
#include <fstream>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#else
#include <csignal>
#endif

#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>

constexpr double NTSC_FPS = 60.0988;
constexpr double PAL_FPS = 50.0070;

NESferatuApp::NESferatuApp(const std::filesystem::path& file_path)
{
    // ========================================================================
    // SDL
    // ========================================================================

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        NESFERATU_THROW("[SDL] Cannot initialize SDL: {}", SDL_GetError());
    }

    window_ = SDL_CreateWindow(
        "NESferatu",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1920, 1080,
        SDL_WINDOW_SHOWN
    );

    if (window_ == nullptr)
    {
        SDL_Quit();
        NESFERATU_THROW("[SDL] Cannot create window: {}", SDL_GetError());
    }

    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    if (renderer_ == nullptr)
    {
        SDL_DestroyWindow(window_);
        SDL_Quit();
        NESFERATU_THROW("[SDL] Cannot create renderer: {}", SDL_GetError());
    }

    texture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, 256, 240);
    if (texture_ == nullptr)
    {
        NESFERATU_THROW("[SDL] Cannot create texture: {}", SDL_GetError());
    }

    // ========================================================================
    // IMGUI
    // ========================================================================

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForSDLRenderer(window_, renderer_);
    ImGui_ImplSDLRenderer2_Init(renderer_);

    ImGui::GetStyle().Alpha = 0.85f;

    // ========================================================================
    // NESFERATU
    // ========================================================================

    cartridge_ = std::make_unique<Cartridge>(file_path);
    nes_ = std::make_unique<System>(*cartridge_);
    nes_->Reset();

    nes_->debugger->SetBreakCallback([this](const DebugContext& context)
    {
        DebuggerBreakCallback(context);
    });

    // ========================================================================
    // AUDIO
    // ========================================================================

    audio_ = std::make_unique<Audio>();
    if (!audio_->Start())
    {
        // TODO: what to do here?
    }

    nes_->apu->SetAudioSampleCallback(&Audio::SampleCallback, audio_.get());
}

NESferatuApp::~NESferatuApp()
{
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    if (texture_ != nullptr)
    {
        SDL_DestroyTexture(texture_);
    }

    if (renderer_ != nullptr)
    {
        SDL_DestroyRenderer(renderer_);
    }

    if (window_ != nullptr)
    {
        SDL_DestroyWindow(window_);
    }

    SDL_Quit();
}

void NESferatuApp::RunApplication()
{
    running_ = true;

    const double target_fps = cartridge_->metadata.cpu_timing == CPUTiming::NTSC ? NTSC_FPS : PAL_FPS;
    const double ticks_per_frame = static_cast<double>(SDL_GetPerformanceFrequency()) / target_fps;
    const Uint64 start = SDL_GetPerformanceCounter();
    double next_frame = static_cast<double>(start);

    static Uint64 last_audio_log = SDL_GetPerformanceCounter();

    u64 samples_per_frame = 0;

    while (running_)
    {
        HandleEvents();
        UpdateControllers();

        while (!nes_->IsFrameComplete() && (step_latch_ || run_latch_))
        {
            if (step_latch_ || run_latch_)
            {
                nes_->Step();
                step_latch_ = false;
            }
        }

        // TODO: clean audio debug stuff
        samples_per_frame = nes_->apu->samples_per_frame;
        nes_->apu->samples_per_frame = 0;

        SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
        SDL_RenderClear(renderer_);

        std::span<const Pixel> framebuffer = nes_->GetFramebuffer();
        SDL_UpdateTexture(texture_, nullptr, framebuffer.data(), 256 * sizeof(Pixel));

        int sdl_width, sdl_height;
        SDL_GetRendererOutputSize(renderer_, &sdl_width, &sdl_height);

        const float win_width = static_cast<float>(sdl_width);
        const float win_height = static_cast<float>(sdl_height);

        const float scale_x = win_width / 256.0f;
        const float scale_y = win_height / 240.0f;
        const float scale = std::min(scale_x, scale_y);

        const float width = 256.0f * scale;
        const float height = 240.0f * scale;

        const SDL_FRect destination {
            (win_width - width) / 2.0f,
            (win_height - height) / 2.0f,
            width, height
        };

        SDL_RenderCopyF(renderer_, texture_, nullptr, &destination);

        if (request_screenshot_)
        {
            SaveFramebuffer(framebuffer);
            request_screenshot_ = false;
        }

        nes_->ClearFrameComplete();

        
        // ========================================================================
        // IMGUI
        // ========================================================================

        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();

        ImGui::NewFrame();

        ImGui::Begin("NESferatu test");
        ImGui::Text("Hello from IMGui");
        ImGui::End();

        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer_);
        
        // ========================================================================
        // END IMGUI
        // ========================================================================

        SDL_RenderPresent(renderer_);
        UpdateFPS();

        next_frame += ticks_per_frame;
        const double frequency = static_cast<double>(SDL_GetPerformanceFrequency());
        for (;;)
        {
            const Uint64 now = SDL_GetPerformanceCounter();
            const double remaining_ticks = next_frame - static_cast<double>(now);

            if (remaining_ticks <= 0.0) break;

            const double remaining_ms = remaining_ticks * 1000.0 / frequency;
            if (remaining_ms > 2.0)
            {
                SDL_Delay(static_cast<Uint32>(remaining_ms - 1.0));
            }
            else
            {
                // Busy-wait
            }
        }

        // Audio underrun/overflow logging
        if (SDL_GetPerformanceCounter() - last_audio_log >= SDL_GetPerformanceFrequency())
        {
            u64 samples = nes_->apu->samples_per_sec;
            nes_->apu->samples_per_sec = 0;
            
            NESFERATU_LOG("[Audio] samples per sec: {}\t samples per frame: {}\tunderruns: {}\toverflows: {}", samples, samples_per_frame, audio_->UnderrunCount(), audio_->OverflowCount());
            last_audio_log = SDL_GetPerformanceCounter();
        }
    }
}

void NESferatuApp::HandleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);

        if (event.type == SDL_QUIT)
        {
            running_ = false;
        }

        if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_s)
        {
            step_latch_ = true;
        }

        if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_r)
        {
            run_latch_ = !run_latch_;
        }

        if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_p)
        {
            request_screenshot_ = true;
        }

        if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_1)
        {
            nes_->ppu->SetDebugViewMode(DebugViewMode::Palette);
        }

        if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_2)
        {
            nes_->ppu->SetDebugViewMode(DebugViewMode::FineX);
        }

        if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_3)
        {
            nes_->ppu->SetDebugViewMode(DebugViewMode::PixelPresence);
        }

        if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_4)
        {
            nes_->ppu->SetDebugViewMode(DebugViewMode::Nametable);
        }

        if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_0)
        {
            nes_->ppu->SetDebugViewMode(DebugViewMode::None);
        }
    }
}

void NESferatuApp::UpdateControllers()
{
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

    nes_->controllers->SetController1Status(status);
}

void NESferatuApp::SaveFramebuffer(std::span<const Pixel> framebuffer)
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

void NESferatuApp::UpdateFPS()
{
    const u64 now = SDL_GetPerformanceCounter();
    if (fps_last_update_ == 0)
    {
        fps_last_update_ = now;
        return;
    }

    ++fps_frame_count_;

    const double elapsed = static_cast<double>(now - fps_last_update_) / static_cast<double>(SDL_GetPerformanceFrequency());
    if (elapsed >= 1.0)
    {
        fps_ = static_cast<double>(fps_frame_count_) / elapsed;
        fps_frame_count_ = 0;
        fps_last_update_ = now;

        const std::string title = std::format("NESferatu - {:.2f} FPS", fps_);
        SDL_SetWindowTitle(window_, title.c_str());
    }
}

void NESferatuApp::DebuggerBreakCallback(const DebugContext &context)
{
    #ifdef _WIN32
    DebugBreak();
    #else
    std::raise(SIGTRAP);
    #endif
}
