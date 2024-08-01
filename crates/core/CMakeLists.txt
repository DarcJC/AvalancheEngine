
avalanche_target(
        SHARED_LIBRARY
        NAME "core"
        SRCS "private/core.cpp" "private/logger.cpp"
)

target_link_libraries(avalanche_core PUBLIC EASTL)
target_link_libraries(avalanche_core PRIVATE spdlog::spdlog)
