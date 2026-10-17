#pragma once

#include <memory>

#include <SDL.h>

#include "entities/system.h"
#include "audio.h"
#include "toast-manager.h"

class NESferatuApp
{
public:
    NESferatuApp(const std::filesystem::path& file_path);
    ~NESferatuApp();

    [[nodiscard]] bool IsRunning() const { return running_; }

    void RunApplication();

private:
    void HandleEvents();
    void UpdateControllers();

    void SaveFramebuffer(std::span<const Pixel> framebuffer);

    void UpdateApplicationBar();

    // UI
    void DrawUI();
    void ProcessShortcuts();

    // Commands
    void OpenROMFile();
    void RunEmulation();
    void StepInstruction();
    void StepFrame();
    void ResetEmulation();

    void DebuggerBreakCallback(const DebugContext& context);

private:
    bool running_ = false;

    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr; 
    SDL_Texture* texture_ = nullptr;

    std::unique_ptr<ToastManager> toast_ = nullptr;

    std::shared_ptr<Cartridge> cartridge_;
    std::unique_ptr<System> nes_;
    std::unique_ptr<Audio> audio_;

    bool run_latch_ = false;
    bool step_instruction_latch_ = false;
    bool step_frame_latch_ = false;
    bool request_screenshot_ = false;

    u64 fps_frame_count_ = 0;
    u64 fps_last_update_ = 0;
    double fps_ = 0.0;
};
