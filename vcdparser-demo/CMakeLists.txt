# SPDX-License-Identifier: MIT

add_executable(vcdparser-demo
        main.cc)

target_include_directories(vcdparser-demo PRIVATE ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(vcdparser-demo vcdparser)
