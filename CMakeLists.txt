cmake_minimum_required(VERSION 3.20)
project(aff2wav_c C)

set(CMAKE_C_STANDARD 11)

include_directories(src)

add_executable(aff2wav_c
        src/main.c
        src/main.h)
