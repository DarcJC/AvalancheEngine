
set(RENDER_DEVICE_SOURCE
        "private/render_device.cpp" "private/render_dependency_graph.cpp"
        "private/render_resource.cpp" "private/render_descriptor.cpp"
        "private/render_frame.cpp"
)

avalanche_target(
        NAME render_device
        SHARED_LIBRARY
        SRCS ${RENDER_DEVICE_SOURCE}
)

target_link_libraries(avalanche_render_device PUBLIC avalanche::core)
target_link_libraries(avalanche_render_device PUBLIC glfw)
