
avalanche_target(
        NAME render_driver_vulkan
        SHARED_LIBRARY
        SRCS "private/vulkan_init.cpp" "private/vulkan_context.cpp"
)

target_link_libraries(avalanche_render_driver_vulkan PUBLIC avalanche::core)
target_link_libraries(avalanche_render_driver_vulkan PUBLIC avalanche::render_device)

if (TARGET Vulkan-Headers)
    target_link_libraries(avalanche_render_driver_vulkan PRIVATE Vulkan-Headers)
elseif (TARGET Vulkan::Headers)
    target_link_libraries(avalanche_render_driver_vulkan PRIVATE Vulkan::Headers)
else ()
    message(FATAL_ERROR "Vulkan-Headers target not found, try to reload cmake project files")
endif ()

target_compile_definitions(avalanche_render_driver_vulkan PRIVATE VULKAN_HPP_DISPATCH_LOADER_DYNAMIC)

add_subdirectory(tests)
