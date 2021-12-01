cmake_minimum_required(VERSION 3.16)
project(AoC C)

set(CMAKE_C_STANDARD 11)

add_executable(Day01 main.c)
target_compile_options(Day01 PUBLIC -Wall -Wextra -Werror -fno-omit-frame-pointer -fsanitize=address)
target_link_options(Day01 PUBLIC -fno-omit-frame-pointer -fsanitize=address)
configure_file(${CMAKE_SOURCE_DIR}/../../data/01.txt ${CMAKE_BINARY_DIR} COPYONLY)
