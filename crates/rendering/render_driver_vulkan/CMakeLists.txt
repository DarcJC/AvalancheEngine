
set(AVALANCHE_RENDER_DERIVER_VULKAN_SOURCE
        "private/vulkan_init.cpp" "private/vulkan_context.cpp" "private/vulkan_debug.cpp"
        "private/vulkan_render_device.cpp" "private/vulkan_window.cpp"
        "private/resource/vulkan_image.cpp" "private/resource/vulkan_format.cpp"
        "private/resource/vulkan_sync.cpp" "private/resource/vulkan_command_buffer.cpp"
        "private/resource/vulkan_surface.cpp" "private/resource/vulkan_command_encoder.cpp"
        "private/resource/vulkan_resource_binding.cpp"
)

avalanche_target(
        NAME render_driver_vulkan
        SHARED_LIBRARY
        SRCS ${AVALANCHE_RENDER_DERIVER_VULKAN_SOURCE}
)

target_link_libraries(avalanche_render_driver_vulkan PUBLIC avalanche::core)
target_link_libraries(avalanche_render_driver_vulkan
        PUBLIC avalanche::render_device
        PRIVATE avalanche::window_server)

if (TARGET Vulkan-Headers)
    target_link_libraries(avalanche_render_driver_vulkan PRIVATE Vulkan-Headers)
elseif (TARGET Vulkan::Headers)
    target_link_libraries(avalanche_render_driver_vulkan PRIVATE Vulkan::Headers)
else ()
    message(FATAL_ERROR "Vulkan-Headers target not found, try to reload cmake project files")
endif ()

target_compile_definitions(avalanche_render_driver_vulkan PRIVATE VULKAN_HPP_DISPATCH_LOADER_DYNAMIC)
