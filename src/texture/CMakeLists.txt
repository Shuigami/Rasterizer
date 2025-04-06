cmake_minimum_required(VERSION 3.21.2)

target_sources(raytracing PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/texture_material.cc
    ${CMAKE_CURRENT_LIST_DIR}/uniform_texture.cc)
