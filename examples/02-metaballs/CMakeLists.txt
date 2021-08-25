cmake_minimum_required(VERSION 3.15)

project(02-metaballs LANGUAGES CXX)

set(THIS ${PROJECT_NAME})

include(${BIG_ROOT}/cmake/Examples.cmake)

set(HEADERS
)

set(SOURCES
    metaballs.cpp
)

add_executable(${THIS} ${HEADERS} ${SOURCES})
USES_EXAMPLES(${THIS})
