set(FILESYSTEM_SOURCE
        "private/filesystem/filesystem.cpp"
        "private/filesystem/system_filesystem.cpp"
)

set(ENGINE_SOURCE
        "private/engine_entry.cpp"
        ${FILESYSTEM_SOURCE}
)

avalanche_target(
        NAME engine
        SHARED_LIBRARY
        SRCS ${ENGINE_SOURCE}
        REFLECTION
)

target_link_libraries(avalanche_engine
        PUBLIC avalanche::core avalanche::window_server "$<LINK_LIBRARY:WHOLE_ARCHIVE,avalanche::render_server>"
)
