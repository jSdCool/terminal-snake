cmake_minimum_required(VERSION 3.22)
project(terminal_snake)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
        argparse
        GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)

add_executable(snake src/snake.cpp src/rogueutil.h src/stopHandler.h src/directional.h src/directional.cpp
        src/inputHandler.cpp
        src/inputHandler.h
        src/timeing.cpp
        src/timeing.h)

target_link_libraries(snake argparse)