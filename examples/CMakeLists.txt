cmake_minimum_required(VERSION 3.15)

project(BigExamples LANGUAGES CXX)

set(THIS ${PROJECT_NAME})

include(${BIG_ROOT}/cmake/Big.cmake)

set(HEADERS
    example_app.h
)

set(SOURCES
    example_app.cpp
)

add_library(${THIS} ${HEADERS} ${SOURCES})
USES_BIG(${THIS})

add_subdirectory(00-helloworld)
add_subdirectory(20-nanovg)