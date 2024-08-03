
if (AVALANCHE_ENABLE_VULKAN_DRIVER_TESTS)
    avalanche_target(
            NAME tests_render_driver_vulkan
            EXECUTABLE
            SRCS "test_main.cpp"
    )
    target_link_libraries(avalanche_tests_render_driver_vulkan PRIVATE avalanche_render_driver_vulkan)
    target_include_directories(avalanche_tests_render_driver_vulkan PRIVATE ../private)
    if (TARGET Vulkan-Headers)
        target_link_libraries(avalanche_tests_render_driver_vulkan PRIVATE Vulkan-Headers)
    elseif (TARGET Vulkan::Headers)
        target_link_libraries(avalanche_tests_render_driver_vulkan PRIVATE Vulkan::Headers)
    endif ()
endif ()
