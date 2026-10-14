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

if (MINGW)
    target_link_options(NESferatu PRIVATE -mconsole)
endif()

set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/third-party/imgui")

target_sources(NESferatu PRIVATE
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer2.cpp
)

target_include_directories(NESferatu PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)
