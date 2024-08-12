
set(sources
        "private/core.cpp"
        "private/logger.cpp"
        "private/resource.cpp"
        "private/container/string.cpp"
        "private/container/allocator.cpp"
        "private/container/exception.cpp"
        "private/execution/graph.cpp"
        "private/execution/executor.cpp"
        "private/execution/coroutine.cpp"
        "private/server_manager.cpp"
)

avalanche_target(
        SHARED_LIBRARY
        NAME "core"
        SRCS ${sources}
)

target_link_libraries(avalanche_core PRIVATE spdlog::spdlog)
target_precompile_headers(avalanche_core PUBLIC public/public_pch.h)
