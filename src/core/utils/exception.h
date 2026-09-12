#pragma once

#include <format>
#include <stdexcept>

class NESferatuException : public std::runtime_error
{
public:
    NESferatuException(
        const std::string& message,
        const char* function,
        const char* file,
        int line
    )
    : std::runtime_error(std::format("[{}:{}] {}: {}", file, line, function, message))
    {}
};

#define NESFERATU_THROW(message, ...) \
    throw NESferatuException( \
        std::format(message __VA_OPT__(,) __VA_ARGS__), \
        __func__, \
        __FILE__, \
        __LINE__)
