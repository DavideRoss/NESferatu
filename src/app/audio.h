#pragma once

#include <array>
#include <atomic>

#include "SDL2/SDL.h"

class Audio
{
public:
    Audio() = default;
    ~Audio();

    Audio(const Audio&) = delete;
    Audio& operator =(const Audio&) = delete;

    bool Start();
    void Stop();

    void PushSample(float sample);

    static void SampleCallback(void* user_data, float sample);

private:
    static constexpr std::size_t BUFFER_SIZE = 8192;
    static constexpr std::size_t BUFFER_MASK = BUFFER_SIZE - 1;
    static_assert((BUFFER_SIZE & BUFFER_MASK) == 0, "Audio buffer size must be a power of two");

    static void SDLAudioCallback(void* user_data, Uint8* stream, int length);

    bool ReadSample(float& sample);

private:
    SDL_AudioDeviceID device_ = 0;

    std::array<float, BUFFER_SIZE> buffer_{};

    alignas(64) std::atomic<std::uint64_t> write_index_{ 0 };
    alignas(64) std::atomic<std::uint64_t> read_index_{ 0 };
};
