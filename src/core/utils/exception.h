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
        int line,
        #ifndef NESFERATU_DISABLE_STACKTRACE
        std::stacktrace trace
        #endif
    )
    : std::runtime_error(
        #ifndef NESFERATU_DISABLE_STACKTRACE
        std::format("[{}:{}] {}: {}\n\nStacktrace:\n{}", file, line, function, message, std::to_string(trace))
        #else
        std::format("[{}:{}] {}: {}", file, line, function, message)
        #endif
    )
    {}
};

#ifndef NESFERATU_DISABLE_STACKTRACE

#define NESFERATU_THROW(message, ...) \
    throw NESferatuException( \
        std::format(message __VA_OPT__(,) __VA_ARGS__), \
        __func__, \
        __FILE__, \
        __LINE__, \
        std::stacktrace::current())

#else

#define NESFERATU_THROW(message, ...) \
    throw NESferatuException( \
        std::format(message __VA_OPT__(,) __VA_ARGS__), \
        __func__, \
        __FILE__, \
        __LINE__)

#endif
