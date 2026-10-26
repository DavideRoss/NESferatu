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

#include <nfd.h>

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

    toast_ = std::make_unique<ToastManager>();

    // ========================================================================
    // NFD
    // ========================================================================

    if (NFD_Init() != NFD_OKAY)
    {
        NESFERATU_THROW("Cannot initialize NFD");
    }

    // ========================================================================
    // NESFERATU
    // ========================================================================

    cartridge_ = std::make_shared<Cartridge>(file_path);
    nes_ = std::make_unique<System>();

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

        while (!nes_->IsFrameComplete() && (step_instruction_latch_ || step_frame_latch_ || run_latch_))
        {
            if (step_instruction_latch_ || step_frame_latch_ || run_latch_)
            {
                nes_->Step();
                step_instruction_latch_ = false;
            }
        }

        // Stop stepping after first frame
        step_frame_latch_ = false;

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

        ProcessShortcuts();
        DrawUI();
        toast_->Render();

        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer_);
        
        // ========================================================================
        // END IMGUI
        // ========================================================================

        SDL_RenderPresent(renderer_);
        UpdateApplicationBar();

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
            step_instruction_latch_ = true;
        }

        if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_r)
        {
            run_latch_ = !run_latch_;
        }

        if (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_p)
        {
            request_screenshot_ = true;
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

void NESferatuApp::UpdateApplicationBar()
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

void NESferatuApp::DrawUI()
{
    ImGuiIO& io = ImGui::GetIO();

    constexpr float HOVER_HEIGHT = 10.0f;
    constexpr float MENU_HEIGHT = 20.0f;

    // TODO: main menu bar should be always visible if no game is running
    const bool show_menu = io.MousePos.y <= HOVER_HEIGHT + MENU_HEIGHT;
    const bool menu_open = ImGui::IsPopupOpen("", ImGuiPopupFlags_AnyPopupId);

    if ((show_menu || menu_open) && ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open...", "Ctrl+O")) ShowOpenROMDialog();
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) running_ = false;

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Emulation"))
        {
            if (ImGui::MenuItem("Run", "F2", run_latch_)) RunEmulation();
            if (ImGui::MenuItem("Step Instruction", "F3")) StepInstruction();
            if (ImGui::MenuItem("Step Frame", "Shift+F3")) StepFrame();
            if (ImGui::MenuItem("Reset", "F4")) ResetEmulation();

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Savestates"))
        {
            if (ImGui::MenuItem("Load Savestate 1", "1")) ReadSavestate(0);
            if (ImGui::MenuItem("Load Savestate 2", "2")) ReadSavestate(1);
            if (ImGui::MenuItem("Load Savestate 3", "3")) ReadSavestate(2);
            if (ImGui::MenuItem("Load Savestate 4", "4")) ReadSavestate(3);
            if (ImGui::MenuItem("Load Savestate 5", "5")) ReadSavestate(4);
            
            ImGui::Separator();
            
            if (ImGui::MenuItem("Save Savestate 1", "Ctrl+1")) WriteSavestate(0);
            if (ImGui::MenuItem("Save Savestate 2", "Ctrl+2")) WriteSavestate(1);
            if (ImGui::MenuItem("Save Savestate 3", "Ctrl+3")) WriteSavestate(2);
            if (ImGui::MenuItem("Save Savestate 4", "Ctrl+4")) WriteSavestate(3);
            if (ImGui::MenuItem("Save Savestate 5", "Ctrl+5")) WriteSavestate(4);

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void NESferatuApp::ProcessShortcuts()
{
    ImGuiIO& io = ImGui::GetIO();

    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_O)) ShowOpenROMDialog();

    if (ImGui::IsKeyPressed(ImGuiKey_F2)) RunEmulation();
    if (ImGui::IsKeyPressed(ImGuiKey_F3)) StepInstruction();
    if (io.KeyShift && ImGui::IsKeyPressed(ImGuiKey_F3)) StepFrame();
    if (ImGui::IsKeyPressed(ImGuiKey_F4)) ResetEmulation();

    // Savestates
    if (ImGui::IsKeyPressed(ImGuiKey_1)) io.KeyCtrl ? WriteSavestate(0) : ReadSavestate(0);
    if (ImGui::IsKeyPressed(ImGuiKey_2)) io.KeyCtrl ? WriteSavestate(1) : ReadSavestate(1);
    if (ImGui::IsKeyPressed(ImGuiKey_3)) io.KeyCtrl ? WriteSavestate(2) : ReadSavestate(2);
    if (ImGui::IsKeyPressed(ImGuiKey_4)) io.KeyCtrl ? WriteSavestate(3) : ReadSavestate(3);
    if (ImGui::IsKeyPressed(ImGuiKey_5)) io.KeyCtrl ? WriteSavestate(4) : ReadSavestate(4);
}

void NESferatuApp::ShowOpenROMDialog()
{
    nfdu8filteritem_t filters [] = { "NES ROM", "nes" };

    nfdopendialogu8args_t args = {};
    args.filterList = filters;
    args.filterCount = 1;
    args.title = "Open NES ROM";

    nfdu8char_t* out_path = nullptr;
    const nfdresult_t result = NFD_OpenDialogU8_With(&out_path, &args);

    if (result == NFD_OKAY)
    {
        OpenROM(std::filesystem::path(out_path));
        NFD_FreePathU8(out_path);
    }
}

void NESferatuApp::RunEmulation()
{
    if (!nes_->IsCartridgeSlotted())
    {
        run_latch_ = false;
        return;
    }

    run_latch_ = !run_latch_;
}

void NESferatuApp::StepInstruction()
{
    if (!nes_->IsCartridgeSlotted())
    {
        step_instruction_latch_ = false;
        return;
    }

    run_latch_ = false;
    step_instruction_latch_ = true;
}

void NESferatuApp::StepFrame()
{
    if (!nes_->IsCartridgeSlotted())
    {
        step_frame_latch_ = false;
        return;
    }

    run_latch_ = false;
    step_frame_latch_ = true;
}

void NESferatuApp::ResetEmulation()
{
    nes_->Reset();
    audio_->Reset();
}

void NESferatuApp::OpenROM(std::filesystem::path path)
{
    cartridge_ = std::make_shared<Cartridge>(path);
    nes_->SlotCartridge(cartridge_);
    audio_->Reset();
    toast_->Show("Cartridge loaded!", 5.0f);
}

void NESferatuApp::ReadSavestate(u32 index)
{
    nes_->ReadSavestate(savestates_[index]);
    toast_->Show(std::format("Savestate {} loaded!", index + 1), 5.0f);
}

void NESferatuApp::WriteSavestate(u32 index)
{
    savestates_[index] = nes_->WriteSavestate();
    toast_->Show(std::format("Savestate {} saved!", index + 1), 5.0f);
}

void NESferatuApp::DebuggerBreakCallback(const DebugContext &context)
{
    #ifdef _WIN32
    DebugBreak();
    #else
    std::raise(SIGTRAP);
    #endif
}
