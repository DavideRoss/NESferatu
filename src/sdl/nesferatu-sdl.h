#pragma once

#include "SDL2/SDL.h"

#include "entities/system.h"

class NESferatuSDL
{
public:
    NESferatuSDL();
    ~NESferatuSDL();

    [[nodiscard]] bool IsRunning() const;

    void PollEvents();
    void Render(std::span<const Pixel> frame);
    void Clear() const;
    void Present() const;

    // Debug function
    void DrawRectangle(Pixel color, int x, int y, int w, int h) const;

private:
    bool running_ = true;

    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr; 
    SDL_Texture* texture_ = nullptr;
};
