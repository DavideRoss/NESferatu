#include "audio.h"

#include <algorithm>
#include <cstring>

#include "utils/log.h"

Audio::~Audio()
{
    Stop();
}

bool Audio::Start()
{
    if (device_ != 0) return true;

    SDL_AudioSpec desired;
    desired.freq = 44100;
    desired.format = AUDIO_F32SYS;
    desired.channels = 1;
    desired.samples = 512;
    desired.callback = SDLAudioCallback;
    desired.userdata = this;

    device_ = SDL_OpenAudioDevice(nullptr, 0, &desired, nullptr, 0);
    if (device_ == 0)
    {
        NESFERATU_LOG("Failed to open audio device: {}", SDL_GetError());
        return false;
    }

    SDL_PauseAudioDevice(device_, 0);
    return true;
}

void Audio::Stop()
{
    if (device_ == 0) return;

    SDL_CloseAudioDevice(device_);
    device_ = 0;
}

void Audio::PushSample(float sample)
{
    const std::uint64_t write = write_index_.load(std::memory_order_relaxed);
    const std::uint64_t read = read_index_.load(std::memory_order_acquire);

    // Buffer is full, drop the sample
    if (write - read >= BUFFER_SIZE) return;

    buffer_[write & BUFFER_MASK] = std::clamp(sample, -1.0f, 1.0f);
    write_index_.store(write + 1, std::memory_order_release);
}

bool Audio::ReadSample(float &sample)
{
    const std::uint64_t read = read_index_.load(std::memory_order_relaxed);
    const std::uint64_t write = write_index_.load(std::memory_order_acquire);

    if (read == write) return false;

    sample = buffer_[read & BUFFER_MASK];

    read_index_.store(read + 1, std::memory_order_release);
    return true;
}

void Audio::SampleCallback(void *user_data, float sample)
{
    Audio* audio = static_cast<Audio*>(user_data);
    audio->PushSample(sample);
}

void Audio::SDLAudioCallback(void *user_data, Uint8 *stream, int length)
{
    Audio* audio = static_cast<Audio*>(user_data);

    std::memset(stream, 0, static_cast<std::size_t>(length));

    float* output = reinterpret_cast<float*>(stream);
    const std::size_t sample_count = static_cast<std::size_t>(length) / sizeof(float);

    for (std::size_t i = 0; i < sample_count; i++)
    {
        float sample;
        if (!audio->ReadSample(sample))
        {
            NESFERATU_LOG("Audio buffer underrun!");
            break;
        }

        output[i] = sample;
    }
}
