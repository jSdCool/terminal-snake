cmake_minimum_required(VERSION 3.22)
project(terminal_snake)

set(CMAKE_CXX_STANDARD 20)

add_executable(snake src/snake.cpp src/rogueutil.h src/stopHandler.h src/directional.h src/directional.cpp)