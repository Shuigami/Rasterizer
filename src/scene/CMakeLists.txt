cmake_minimum_required(VERSION 3.21.2)

target_sources(toon PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/camera.cc
    ${CMAKE_CURRENT_LIST_DIR}/scene.cc)
