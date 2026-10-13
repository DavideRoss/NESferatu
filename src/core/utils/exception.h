#pragma once

#include <iostream>
#include <format>
#include <stdexcept>

#ifndef NESFERATU_DISABLE_STACKTRACE
#include <stacktrace>
#endif

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

#ifdef NESFERATU_DISABLE_STACKTRACE

#define NESFERATU_THROW(message, ...) \
    throw NESferatuException( \
        std::format(message __VA_OPT__(,) __VA_ARGS__), \
        __func__, \
        __FILE__, \
        __LINE__)

#else

#define NESFERATU_THROW(message, ...) \
    auto trace = std::stacktrace::current();\
    std::cout << trace << '\n';\
    throw NESferatuException( \
        std::format(message __VA_OPT__(,) __VA_ARGS__), \
        __func__, \
        __FILE__, \
        __LINE__)

#endif
