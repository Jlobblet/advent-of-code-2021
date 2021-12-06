cmake_minimum_required(VERSION 3.16)
project(AoC C)

set(CMAKE_C_STANDARD 11)
list(APPEND CompileOptions -Wall -Wextra)
list(APPEND AsanOptions -fno-omit-frame-pointer -fsanitize=address)

if (CMAKE_BUILD_TYPE MATCHES DEBUG)
    list(APPEND CompileOptions ${AsanOptions})
    list(APPEND LinkOptions ${AsanOptions})
else ()
    list(APPEND CompileOptions -Werror)
endif ()

configure_file(${CMAKE_SOURCE_DIR}/../../data/01.txt ${CMAKE_BINARY_DIR}/src/data/01.txt COPYONLY)

add_executable(Day01 day01.c)
target_compile_options(Day01 PUBLIC ${CompileOptions})
target_link_libraries(Day01 JC)
target_link_options(Day01 PUBLIC ${LinkOptions})

add_executable(Day01f day01f.c)
target_compile_options(Day01f PUBLIC ${CompileOptions})
target_link_libraries(Day01f JC)
target_link_options(Day01f PUBLIC ${LinkOptions})

configure_file(${CMAKE_SOURCE_DIR}/../../data/02.txt ${CMAKE_BINARY_DIR}/src/data/02.txt COPYONLY)

add_executable(Day02 day02.c)
target_compile_options(Day02 PUBLIC ${CompileOptions})
target_link_libraries(Day02 JC)
target_link_options(Day02 PUBLIC ${LinkOptions})

configure_file(${CMAKE_SOURCE_DIR}/../../data/03.txt ${CMAKE_BINARY_DIR}/src/data/03.txt COPYONLY)

add_executable(Day03 day03.c)
target_compile_options(Day03 PUBLIC ${CompileOptions})
target_link_libraries(Day03 JC)
target_link_options(Day03 PUBLIC ${LinkOptions})
