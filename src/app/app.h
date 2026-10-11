#pragma once

#include <memory>

#include "SDL2/SDL.h"

#include "entities/system.h"

class NESferatuApp
{
public:
    NESferatuApp();
    ~NESferatuApp();

    [[nodiscard]] bool IsRunning() const { return running_; }

    void RunApplication();

private:
    void HandleEvents();
    void UpdateControllers();

    void SaveFramebuffer(std::span<const Pixel> framebuffer);

    void DebuggerBreakCallback(const DebugContext& context);

private:
    bool running_ = false;

    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr; 
    SDL_Texture* texture_ = nullptr;

    std::unique_ptr<Cartridge> cartridge_;
    std::unique_ptr<System> nes_;

    bool run_latch_ = true;
    bool step_latch_ = false;
    bool request_screenshot_ = false;
};
