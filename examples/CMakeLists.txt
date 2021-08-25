cmake_minimum_required(VERSION 3.15)

project(BigExamples LANGUAGES CXX)

set(THIS ${PROJECT_NAME})

include(${BIG_ROOT}/cmake/Big.cmake)
include(${BIG_ROOT}/cmake/Utils.cmake)

set(HEADERS
    example_app.h
)

set(SOURCES
    example_app.cpp
    example_dialog.cpp
)

add_library(${THIS} ${HEADERS} ${SOURCES})
USES_BIG(${THIS})
USES_UTILS(${THIS})

add_subdirectory(00-helloworld)
add_subdirectory(01-cubes)
add_subdirectory(02-metaballs)
add_subdirectory(03-raymarch)
add_subdirectory(04-mesh)
add_subdirectory(20-nanovg)