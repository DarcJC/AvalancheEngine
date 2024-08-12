
avalanche_target(
        NAME render_device
        SHARED_LIBRARY
        SRCS "private/render_device.cpp" "private/render_dependency_graph.cpp"
)

target_link_libraries(avalanche_render_device PUBLIC avalanche::core)
target_link_libraries(avalanche_render_device PUBLIC glfw)
