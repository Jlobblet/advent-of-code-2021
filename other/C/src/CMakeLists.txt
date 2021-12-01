cmake_minimum_required(VERSION 3.16)
project(AoC C)

set(CMAKE_C_STANDARD 11)
list(APPEND CompileOptions -Wall -Wextra -Werror)
list(APPEND AsanOptions -fno-omit-frame-pointer -fsanitize=address)

if (CMAKE_BUILD_TYPE MATCHES DEBUG)
    list(APPEND CompileOptions ${AsanOptions})
    list(APPEND LinkOptions ${AsanOptions})
endif ()

configure_file(${CMAKE_SOURCE_DIR}/../../data/01.txt ${CMAKE_BINARY_DIR}/src/data/01.txt COPYONLY)

add_executable(Day01 day01.c common.c common.h)
target_compile_options(Day01 PUBLIC ${CompileOptions})
target_link_options(Day01 PUBLIC ${LinkOptions})

add_executable(Day01f day01f.c common.c common.h)
target_compile_options(Day01f PUBLIC ${CompileOptions})
target_link_options(Day01f PUBLIC ${LinkOptions})

