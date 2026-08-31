find_package(SDL2 CONFIG REQUIRED)

file(GLOB_RECURSE SDL_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_library(NESferatuSDL STATIC
    ${SDL_SOURCES}
)

target_include_directories(NESferatuSDL PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(NESferatuSDL
    PUBLIC
        NESferatuCore
        
    PRIVATE
        SDL2::SDL2
        NESferatuCompileOptions
)
