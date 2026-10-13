file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_library(NESferatuCore STATIC
    ${LIB_SOURCES}
)

target_include_directories(NESferatuCore PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(NESferatuCore PRIVATE
    NESferatuCompileOptions
    NESferatuLinkerOptions
)

if (NOT MINGW)
    target_link_libraries(NESferatuCore PRIVATE
        stdc++_libbacktrace
    )
endif()

if (MINGW)
    target_compile_definitions(NESferatuCore PUBLIC
        NESFERATU_DISABLE_STACKTRACE
    )
endif()
