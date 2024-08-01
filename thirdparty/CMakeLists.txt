
if (AVALANCHE_ENABLE_VULKAN AND NOT TARGET Vulkan-Headers AND NOT TARGET Vulkan::Headers)
    add_subdirectory(Vulkan-Headers)
endif ()

set(SPDLOG_BUILD_SHARED OFF)
add_subdirectory(spdlog)

add_subdirectory(eastl)
