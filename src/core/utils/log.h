#pragma once

#include <iostream>
#include <format>

#define NESFERATU_LOG(message, ...) \
    std::cout << std::format(message __VA_OPT__(,) __VA_ARGS__) << '\n'
