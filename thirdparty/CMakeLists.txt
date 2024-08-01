
if (AVALANCHE_ENABLE_VULKAN)
    if (NOT TARGET Vulkan-Headers OR NOT TARGET Vulkan::Headers)
        add_subdirectory(Vulkan-Headers)
    endif ()
endif ()

set(SPDLOG_BUILD_SHARED OFF)
add_subdirectory(spdlog)

add_subdirectory(glfw)
