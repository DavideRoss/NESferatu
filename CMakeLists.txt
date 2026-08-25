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

# ===========================================================
# DEPENDENCIES
# ===========================================================

find_package(SDL2 CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

enable_testing()

# ===========================================================
# LIB
# ===========================================================

file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

list(REMOVE_ITEM LIB_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
)

add_library(NESferatuLib
    ${LIB_SOURCES}
)

target_include_directories(NESferatuLib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# ===========================================================
# EXECUTABLE
# ===========================================================

add_executable(NESferatu src/main.cpp)

target_link_libraries(NESferatu PRIVATE
    NESferatuLib
    SDL2::SDL2
    SDL2::SDL2main
)

# ===========================================================
# TESTS
# ===========================================================

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
)

add_executable(NESferatuTests
    ${TEST_SOURCES}
)

target_include_directories(NESferatuTests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/tests"
)

target_link_libraries(NESferatuTests PRIVATE
    NESferatuLib
    GTest::gtest
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(NESferatuTests)

# ===========================================================
# COMPILER
# ===========================================================

set(COMMON_WARNINGS
    -Wall
    -Wextra
    -Werror
    -Wcast-qual
    -Wmissing-declarations
    -Woverlength-strings
    -Wwrite-strings
)

target_compile_options(NESferatu PRIVATE ${COMMON_WARNINGS})
target_compile_options(NESferatuLib PRIVATE ${COMMON_WARNINGS})

target_compile_options(NESferatuTests PRIVATE ${COMMON_WARNINGS}
    -Wno-unused-variable
    -Wno-missing-declarations
)