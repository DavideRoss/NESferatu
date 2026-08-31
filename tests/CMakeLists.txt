find_package(GTest CONFIG REQUIRED)

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_executable(NESferatuTests
    ${TEST_SOURCES}
)

target_include_directories(NESferatuTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(NESferatuTests PRIVATE
    NESferatuCore
    GTest::gtest
    GTest::gtest_main
)

target_compile_options(NESferatuTests PRIVATE
        -Wall
        -Wextra
        -Werror
        -Wcast-qual
        -Wmissing-declarations
        -Woverlength-strings
        -Wwrite-strings

        -Wno-unused-variable
        -Wno-missing-declarations
)

gtest_discover_tests(NESferatuTests)
