
set(SPDLOG_BUILD_SHARED OFF)
add_subdirectory(spdlog)

add_subdirectory(glfw)

add_subdirectory(dylib)

add_subdirectory(slang)

if (AVALANCHE_ENABLE_VULKAN)
    # The slang will introduce Vulkan-Headers but we don't want to rely on a black box.
    if (NOT TARGET Vulkan-Headers OR NOT TARGET Vulkan::Headers)
        add_subdirectory(Vulkan-Headers)
    endif ()
endif ()
