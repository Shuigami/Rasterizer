cmake_minimum_required(VERSION 3.21.2)

target_sources(toon PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/cube.cc
    ${CMAKE_CURRENT_LIST_DIR}/object.cc
    ${CMAKE_CURRENT_LIST_DIR}/sphere.cc
    ${CMAKE_CURRENT_LIST_DIR}/plane.cc
    ${CMAKE_CURRENT_LIST_DIR}/triangle.cc)
