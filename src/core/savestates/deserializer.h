#pragma once

#include <array>

#include "utils/exception.h"
#include "utils/types.h"

class Deserializer
{
public:
    explicit Deserializer(std::span<const std::byte> data)
        : data_(data)
    {}

    std::size_t Remaining() const noexcept
    {
        return data_.size() - position_;
    }

    template <typename T>
        requires (std::is_integral_v<T> && !std::is_same_v<T, bool>)
    void Read(T& value)
    {
        using U = std::make_unsigned_t<T>;

        if (Remaining() < sizeof(T))
        {
            NESFERATU_THROW("Unexpected end of data");
        }

        U bits = 0;
        for (std::size_t i = 0; i < sizeof(T); ++i)
        {
            const U byte = static_cast<U>(std::to_integer<unsigned char>(data_[position_]));
            position_++;

            bits |= byte << (i * 8);
        }

        if constexpr (std::is_signed_v<T>)
        {
            value = std::bit_cast<T>(bits);
        }
        else
        {
            value = bits;
        }
    }

    template <typename T>
        requires std::is_enum_v<T>
    void Read(T& value)
    {
        using U = std::underlying_type_t<T>;

        U raw;
        Read(raw);
        value = static_cast<T>(raw);
    }

    template <typename T, std::size_t N>
    void Read(std::array<T, N>& dest)
    {
        std::span<std::byte> bytes = std::as_writable_bytes(std::span{ dest });

        if (Remaining() < bytes.size())
        {
            NESFERATU_THROW("Unexpected end of save state");
        }

        std::copy_n(data_.begin() + position_, bytes.size(), bytes.begin());
        position_ += bytes.size();
    }

    void Read(bool& value)
    {
        u8 raw;
        Read(raw);

        value = raw != 0;
    }

    void Read(float& value)
    {
        u32 bits;
        Read(bits);
        value = static_cast<float>(bits);
    }

private:
    std::span<const std::byte> data_;

    std::size_t position_ = 0;
};
