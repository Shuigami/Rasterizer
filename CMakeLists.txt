cmake_minimum_required(VERSION 3.10)
project(SoftwareRasterizer VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})
include_directories(include)

add_library(logger SHARED
    src/logger.cpp
)
target_include_directories(logger PUBLIC include)

set(SOURCES
    src/main.cpp
    src/rasterizer.cpp
    src/mesh.cpp
    src/camera.cpp
    src/shader.cpp
)

add_executable(rasterizer ${SOURCES})
target_link_libraries(rasterizer logger SDL2 SDL2_image)

file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
