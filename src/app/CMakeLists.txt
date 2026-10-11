find_package(SDL2 CONFIG REQUIRED)

file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_executable(NESferatu
    ${APP_SOURCES}
)

target_link_libraries(NESferatu
    PUBLIC
        NESferatuCore
    PRIVATE
        SDL2::SDL2
        NESferatuCompileOptions
        NESferatuLinkerOptions
)