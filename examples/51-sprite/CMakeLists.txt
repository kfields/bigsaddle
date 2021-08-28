cmake_minimum_required(VERSION 3.15)

project(51-sprite LANGUAGES CXX)

set(THIS ${PROJECT_NAME})

include(${BIG_ROOT}/cmake/Examples.cmake)

set(HEADERS
)

set(SOURCES
    sprite.cpp
)

add_example(${THIS} ${HEADERS} ${SOURCES})
