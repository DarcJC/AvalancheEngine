set(ENGINE_SOURCE
        "private/engine_entry.cpp"
)

avalanche_target(
        NAME engine
        SHARED_LIBRARY
        SRCS ${ENGINE_SOURCE}
)

target_link_libraries(avalanche_engine
        PUBLIC avalanche::core avalanche::window_server "$<LINK_LIBRARY:WHOLE_ARCHIVE,avalanche::render_server>"
)
