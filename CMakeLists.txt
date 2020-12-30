# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.0)
project(libvcdparse)

set(CMAKE_CXX_STANDARD 11)

add_library(vcdparser
        src/libvcdparser.cc
        src/tokenizer.cc
        src/utils.cc)

add_subdirectory(vcdparser-demo)