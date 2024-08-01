
avalanche_target(
        NAME render_driver_vulkan
        SHARED_LIBRARY
        SRCS "private/vulkan_init.cpp"
)

target_link_libraries(avalanche_render_driver_vulkan PUBLIC avalanche::core)
target_link_libraries(avalanche_render_driver_vulkan PRIVATE avalanche::render_device)

if (TARGET Vulkan-Headers)
    target_link_libraries(avalanche_render_driver_vulkan PRIVATE Vulkan-Headers)
elseif (TARGET Vulkan::Headers)
    target_link_libraries(avalanche_render_driver_vulkan PRIVATE Vulkan::Headers)
else ()
    message(FATAL_ERROR "Vulkan-Headers target not found")
endif ()

target_compile_definitions(avalanche_render_driver_vulkan PRIVATE VULKAN_HPP_DISPATCH_LOADER_DYNAMIC)
