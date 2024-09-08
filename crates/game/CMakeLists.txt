set(GAME_SOURCE
        "private/game_main.cpp"
)

avalanche_target(
        NAME game
        EXECUTABLE
        SRCS ${GAME_SOURCE}
)

target_link_libraries(avalanche_game
        PUBLIC avalanche::engine
)
