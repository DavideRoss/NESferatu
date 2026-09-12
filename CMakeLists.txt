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

add_library(NESferatuCompileOptions INTERFACE)

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

include(GoogleTest)
enable_testing()

add_subdirectory(src)
add_subdirectory(tests)
