#pragma once

#include <array>
#include <atomic>

#include <SDL.h>

class Audio
{
public:
    Audio();
    ~Audio();

    Audio(const Audio&) = delete;
    Audio& operator =(const Audio&) = delete;

    bool Start();
    void Stop();
    void Reset();

    void PushSample(float sample);

    uint64_t OverflowCount() const { return overflow_count_.load(std::memory_order_relaxed); }
    uint64_t UnderrunCount() const { return underrun_count_.load(std::memory_order_relaxed); }

    static void SampleCallback(void* user_data, float sample);

private:
    static constexpr size_t BUFFER_SIZE = 8192;
    static constexpr size_t BUFFER_MASK = BUFFER_SIZE - 1;
    static_assert((BUFFER_SIZE & BUFFER_MASK) == 0, "Audio buffer size must be a power of two");

    static void SDLAudioCallback(void* user_data, Uint8* stream, int length);

    bool ReadSample(float& sample);

private:
    SDL_AudioDeviceID device_ = 0;

    std::array<float, BUFFER_SIZE> buffer_{};

    alignas(64) std::atomic<uint64_t> write_index_{ 0 };
    alignas(64) std::atomic<uint64_t> read_index_{ 0 };

    std::atomic<uint64_t> overflow_count_{ 0 };
    std::atomic<uint64_t> underrun_count_{ 0 };
};
