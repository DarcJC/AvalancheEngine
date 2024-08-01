
avalanche_target(
        SHARED_LIBRARY
        NAME "core"
        SRCS "private/core.cpp" "private/logger.cpp"
)

target_link_libraries(avalanche_core PRIVATE spdlog::spdlog)
target_precompile_headers(avalanche_core PUBLIC public/public_pch.h)
