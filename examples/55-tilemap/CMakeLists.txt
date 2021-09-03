cmake_minimum_required(VERSION 3.15)

project(55-tilemap LANGUAGES CXX)

set(THIS ${PROJECT_NAME})

include(${BIG_ROOT}/cmake/Examples.cmake)

set(HEADERS
)

set(SOURCES
    tilemap.cpp
)

add_example(${THIS} ${HEADERS} ${SOURCES})

