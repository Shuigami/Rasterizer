cmake_minimum_required(VERSION 3.21.2)
project(toon)

add_executable(toon main.cc)
target_compile_options(toon PRIVATE -Wall -Wextra -Werror -std=c++20 -pedantic -Wold-style-cast -g -O0)
target_include_directories(toon PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(src)
