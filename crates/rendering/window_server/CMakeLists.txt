
avalanche_target(
        NAME window_server
        STATIC_LIBRARY
        SRCS "private/window_server.cpp" "private/window.cpp"
        REFLECTION
)

target_link_libraries(avalanche_window_server
        PUBLIC avalanche::core avalanche::render_device)
target_link_libraries(avalanche_window_server PRIVATE glfw)
