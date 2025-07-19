cmake_minimum_required(VERSION 3.2)

project(waychess VERSION 1.0.0)

add_compile_options(-Wall -Wextra -Wpedantic)

add_subdirectory(src)
add_subdirectory(apps)

include(CTest)
add_subdirectory(tests)