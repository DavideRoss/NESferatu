#include "app.h"

#include <stdexcept>
#include <format>
#include <csignal>
#include <fstream>

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

        SDL_RenderPresent(renderer_);

        // TODO: write proper timing
        SDL_Delay(4);
    }
}

void NESferatuApp::HandleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
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

void NESferatuApp::DebuggerBreakCallback(const DebugContext &context)
{
    std::raise(SIGTRAP);
}
