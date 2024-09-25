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

# Copy dynamic library to our binary directory
add_custom_command(TARGET avalanche_shader_server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:slang> $<TARGET_FILE_DIR:avalanche_shader_server>
)
