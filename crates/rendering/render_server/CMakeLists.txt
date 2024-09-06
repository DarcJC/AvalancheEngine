
avalanche_target(
        NAME render_server
        SHARED_LIBRARY
        SRCS "private/render_server.cpp"
)

target_link_libraries(avalanche_render_server PUBLIC avalanche::core avalanche::render_device)


