cmake_minimum_required(VERSION 3.0)
project(libvcdparse)

set(CMAKE_CXX_STANDARD 17)

add_library(vcdparser
        src/libvcdparser.cc
        src/tokenizer.cc
        src/utils.cc)

add_executable(vcdparser-demo
        vcdparser-demo/main.cc)

target_include_directories(vcdparser-demo PRIVATE src/)

target_link_libraries(vcdparser-demo vcdparser)
