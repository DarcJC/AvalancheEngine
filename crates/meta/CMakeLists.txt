set(META_SOURCES
        "private/class.cpp" "private/metaspace.cpp"
)

avalanche_target(
        SHARED_LIBRARY
        NAME "meta"
        SRCS ${META_SOURCES}
)

target_link_libraries(avalanche_meta)
