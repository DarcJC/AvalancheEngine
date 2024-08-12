
avalanche_target(
        NAME window_server
        STATIC_LIBRARY
        SRCS "private/window_server.cpp"
)

target_link_libraries(avalanche_window_server PUBLIC avalanche::core)
target_link_libraries(avalanche_window_server PRIVATE glfw)
