cmake_minimum_required(VERSION 3.28)
project(minesweeper)

set(CMAKE_CXX_STANDARD 17)

#add_compile_definitions(NO_COLORS)

add_executable(minesweeper src/main.cpp
        include/ui.hpp
        src/ui.cpp
        include/board.hpp
        src/board.cpp
        include/rng.hpp
        include/timer.hpp
        src/timer.cpp
)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
target_link_libraries(minesweeper ${CURSES_LIBRARIES})

include(FetchContent)
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)
target_link_libraries(minesweeper argparse)
