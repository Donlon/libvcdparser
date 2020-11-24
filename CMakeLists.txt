cmake_minimum_required(VERSION 3.0)
project(libvcdparse)

set(CMAKE_CXX_STANDARD 17)

add_library(libvcdparser
        src/libvcdparser.cc
        src/tokenizer.cc)

add_executable(vcdparser-demo
        vcdparser-demo/main.cc)