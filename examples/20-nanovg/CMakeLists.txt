cmake_minimum_required(VERSION 3.15)

project(20-nanovg LANGUAGES CXX)

set(THIS ${PROJECT_NAME})

include(../../cmake/Big.cmake)
include(../../cmake/NanoVG.cmake)

set(BIG_HEADERS
    main.h
)

set(BIG_SRC
    main.cpp
    util.cpp
    window.cpp
    searchbox.cpp
    graph.cpp
)

add_executable(${THIS} ${BIG_SRC} ${BIG_HEADERS})
USES_BIG(${THIS})
USES_NVG(${THIS})
