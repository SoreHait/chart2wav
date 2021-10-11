cmake_minimum_required(VERSION 3.20)
project(aff2wav_c C)

set(CMAKE_C_STANDARD 11)

include_directories(
        src
        src/module
        src/util
)

aux_source_directory(src main)
aux_source_directory(src/module mod)
aux_source_directory(src/util util)

add_executable(aff2wav_c
        ${main}
        ${mod}
        ${util}
        )
