cmake_minimum_required(VERSION 3.10)
project(SoftwareRasterizer VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

include_directories(include)

set(SOURCES
    src/main.cpp
    src/rasterizer.cpp
    src/mesh.cpp
    src/camera.cpp
    src/shader.cpp
)

if(EMSCRIPTEN)
    # Web build: emcmake cmake -B build-web && cmake --build build-web
    # Produces index.html + index.js + index.wasm + index.data in build-web/.
    add_library(logger STATIC src/logger.cpp)
    target_include_directories(logger PUBLIC include)

    add_executable(rasterizer ${SOURCES})
    set_target_properties(rasterizer PROPERTIES OUTPUT_NAME "index" SUFFIX ".html")
    target_compile_options(rasterizer PRIVATE -O3 "SHELL:-s USE_SDL=2")
    target_compile_options(logger PRIVATE -O3)
    target_link_libraries(rasterizer logger)
    target_link_options(rasterizer PRIVATE
        -O3
        "SHELL:-s USE_SDL=2"
        "SHELL:-s ALLOW_MEMORY_GROWTH=1"
        "SHELL:-s INITIAL_MEMORY=67108864"
        "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/assets/well.obj@assets/well.obj"
        "SHELL:--shell-file ${CMAKE_SOURCE_DIR}/web/shell.html"
    )
    set_property(TARGET rasterizer APPEND PROPERTY LINK_DEPENDS
        ${CMAKE_SOURCE_DIR}/web/shell.html)
else()
    find_package(SDL2 REQUIRED)
    include_directories(${SDL2_INCLUDE_DIRS})

    add_library(logger SHARED src/logger.cpp)
    target_include_directories(logger PUBLIC include)

    add_executable(rasterizer ${SOURCES})
    target_link_libraries(rasterizer logger SDL2)

    file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
endif()
