
avalanche_target(
        NAME render_driver_vulkan
        SHARED_LIBRARY
        SRCS "private/vulkan_init.cpp"
)

target_link_libraries(avalanche_render_driver_vulkan PUBLIC avalanche::core)
target_link_libraries(avalanche_render_driver_vulkan PRIVATE Vulkan-Headers avalanche::render_device)
