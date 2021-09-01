cmake_minimum_required(VERSION 3.15)

project(54-tileset LANGUAGES CXX)

set(THIS ${PROJECT_NAME})

include(${BIG_ROOT}/cmake/Examples.cmake)
include(${BIG_ROOT}/cmake/Pugi.cmake)

set(HEADERS
)

set(SOURCES
    tileset.cpp
)

add_example(${THIS} ${HEADERS} ${SOURCES})
USES_PUGI(${THIS})
