#pragma once

#include <array>
#include <span>
#include <vector>

#include "utils/types.h"


// TODO: add magic number and header (NESferatu core version?)
class Serializer
{
public:
    using Buffer = std::vector<std::byte>;

    const Buffer& Data() const noexcept
    {
        return data_;
    }

    template <typename T>
        requires (std::is_integral_v<T> && !std::is_same_v<T, bool>)
    void Write(T value)
    {
        using U = std::make_unsigned_t<T>;

        U bits;
        if constexpr (std::is_signed_v<T>)
        {
            bits = std::bit_cast<U>(value);
        }
        else
        {
            bits = value;
        }

        for (std::size_t i = 0; i < sizeof(T); ++i)
        {
            data_.push_back(static_cast<std::byte>(bits & U{ 0xff }));
            bits >>= 8;
        }
    }

    template <typename T>
        requires std::is_enum_v<T>
    void Write(T value)
    {
        Write(static_cast<std::underlying_type_t<T>>(value));
    }

    template <typename T, std::size_t N>
        requires std::is_trivially_copyable_v<T>
    void Write(const std::span<T, N>& value)
    {
        const std::span<const std::byte> bytes = std::as_bytes(std::span{ value });
        data_.insert(data_.end(), bytes.begin(), bytes.end());
    }

    template <typename T, std::size_t N>
        requires std::is_trivially_copyable_v<T>
    void Write(const std::array<T, N>& value)
    {
        Write(std::span{ value });
    }

    void Write(bool value)
    {
        Write(static_cast<u8>(value ? 1 : 0));
    }

    void Write(float value)
    {
        Write(std::bit_cast<u32>(value));
    }

private:
    Buffer data_;
};
