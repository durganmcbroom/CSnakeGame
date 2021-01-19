cmake_minimum_required(VERSION 3.17)
project(CSnakeGame C)

set(CMAKE_C_STANDARD 99)

add_executable(CSnakeGame main.c)

target_link_libraries(CSnakeGame -lncurses)