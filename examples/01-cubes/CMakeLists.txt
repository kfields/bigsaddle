cmake_minimum_required(VERSION 3.15)

project(01-cubes LANGUAGES CXX)

set(THIS ${PROJECT_NAME})

include(${BIG_ROOT}/cmake/Examples.cmake)

set(HEADERS
)

set(SOURCES
    cubes.cpp
)

add_executable(${THIS} ${HEADERS} ${SOURCES})
USES_EXAMPLES(${THIS})
