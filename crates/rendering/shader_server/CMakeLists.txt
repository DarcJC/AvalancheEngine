set(SHADER_SEVER_SOURCES
        "private/shader_server.cpp" "private/shader_compiler.cpp"
)

avalanche_target(
        NAME shader_server
        SHARED_LIBRARY
        SRCS "${SHADER_SEVER_SOURCES}"
        REFLECTION
)

target_link_libraries(avalanche_shader_server
        PUBLIC
        avalanche::core
        avalanche::render_device
)

target_link_libraries(avalanche_shader_server
        PRIVATE
        slang
        core
        compiler-core
)
