cmake_minimum_required(VERSION 3.2)
project(Minesweeper)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SFML_DIR "C:/Dev/SFML-2.5.1/lib/cmake/SFML")

find_package(SFML 2.5.1 COMPONENTS system window graphics audio REQUIRED)

add_executable(Minesweeper
    src/main.cpp
    src/Tile.cpp
    src/MineField.cpp
)

target_link_libraries(Minesweeper
    sfml-system
    sfml-window
    sfml-graphics
    sfml-audio
)

# Copy assets to build directory automatically
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY config.cfg DESTINATION ${CMAKE_BINARY_DIR})
file(COPY leaderboard.txt DESTINATION ${CMAKE_BINARY_DIR})
