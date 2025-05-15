cmake_minimum_required(VERSION 3.10)
project(SoftwareRasterizer VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find SDL2 and SDL2_image
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

# Include SDL2 and SDL2_image headers
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/rasterizer.cpp
    src/mesh.cpp
    src/camera.cpp
    src/shader.cpp
)

# # Define executable and link with SDL2 and SDL2_image
# add_executable(rasterizer ${SOURCES})
# target_link_libraries(rasterizer ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})
# Define executable and link with SDL2 and SDL2_image
add_executable(rasterizer ${SOURCES})
target_link_libraries(rasterizer SDL2 SDL2_image)

# Copy assets directory
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
