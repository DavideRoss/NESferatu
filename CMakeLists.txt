cmake_minimum_required(VERSION 3.22)

project(
    NESferatu
    VERSION "0.0.1"
    DESCRIPTION "NES emulator"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_BINARY_DIR}/bin
)

option(NESFERATU_BUILD_TESTS "Build tests" ON)
option(NESFERATU_SANITIZERS "Enable ASan and UBSan" OFF)

add_library(NESferatuCompileOptions INTERFACE)
add_library(NESferatuLinkerOptions INTERFACE)

target_compile_options(NESferatuCompileOptions INTERFACE
    -Wall
    -Wextra
    -Werror
    -Wcast-qual
    -Wmissing-declarations
    -Woverlength-strings
    -Wwrite-strings
    
    # TODO: enable unused check for final compilation
    -Wno-unused-parameter
    -Wno-unused-variable

    -fmacro-prefix-map=${CMAKE_SOURCE_DIR}/=
)

if (NESFERATU_SANITIZERS)
    target_compile_options(NESferatuCompileOptions INTERFACE
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
    )

    target_link_options(NESferatuLinkerOptions INTERFACE
        -fsanitize=address,undefined
    )
endif()

if (NESFERATU_BUILD_TESTS)
    include(GoogleTest)
    enable_testing()
    add_subdirectory(tests)
endif()

add_subdirectory(src)
