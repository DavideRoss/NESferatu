#include "nesferatu-sdl.h"

#include <stdexcept>
#include <format>

NESferatuSDL::NESferatuSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        throw std::runtime_error(std::format("[SDL] Cannot initialize SDL: {}", SDL_GetError()));
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
        throw std::runtime_error(std::format("[SDL] Cannot create window: {}", SDL_GetError()));
    }

    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    if (renderer_ == nullptr)
    {
        SDL_DestroyWindow(window_);
        SDL_Quit();
        throw std::runtime_error(std::format("[SDL] Cannot create renderer: {}", SDL_GetError()));
    }

    texture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, 256, 240);
    if (texture_ == nullptr)
    {
        throw std::runtime_error(std::format("[SDL] Cannot create texture: {}", SDL_GetError()));
    }
}

NESferatuSDL::~NESferatuSDL()
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

bool NESferatuSDL::IsRunning() const
{
    return running_;
}

void NESferatuSDL::PollEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            running_ = false;
        }
    }
}

void NESferatuSDL::Render(std::span<const Pixel> frame)
{
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);

    // ========================================================================
    // RENDERING
    // ========================================================================

    SDL_UpdateTexture(texture_, nullptr, frame.data(), 256 * sizeof(Pixel));

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

    // ========================================================================
    // PRESENT
    // ========================================================================

    SDL_RenderPresent(renderer_);
}