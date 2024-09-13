
avalanche_target(
        NAME render_server
        SHARED_LIBRARY
        SRCS "private/render_server.cpp"
        REFLECTION
)

target_link_libraries(avalanche_render_server
        PUBLIC
        avalanche::core
        PRIVATE
        avalanche::render_device
)

if (AVALANCHE_ENABLE_VULKAN)
    target_link_libraries(avalanche_render_server PRIVATE
            avalanche::render_driver_vulkan
    )
    target_compile_definitions(avalanche_render_server PUBLIC
            AVALANCHE_ENABLE_VULKAN=1
    )
endif ()
