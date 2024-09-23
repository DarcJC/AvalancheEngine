avalanche_target(
        NAME shader_server
        SHARED_LIBRARY
        SRCS "private/shader_server.cpp"
        REFLECTION
)

target_link_libraries(avalanche_shader_server
        PUBLIC
        avalanche::core
        PRIVATE
        avalanche::render_device
)

target_link_libraries(avalanche_shader_server
        PRIVATE
        slang
)
