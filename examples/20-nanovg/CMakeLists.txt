cmake_minimum_required(VERSION 3.15)

project(20-nanovg LANGUAGES CXX)

set(THIS ${PROJECT_NAME})

include(${BIG_ROOT}/cmake/Examples.cmake)
include(${BIG_ROOT}/cmake/NanoVG.cmake)

set(BIG_HEADERS
    main.h
)

set(BIG_SRC
    main.cpp
    data.cpp
    util.cpp
    window.cpp
    searchbox.cpp
    eyes.cpp
    paragraph.cpp
    lines.cpp
    graph.cpp
    colorwheel.cpp
    scissor.cpp
)

add_executable(${THIS} ${BIG_SRC} ${BIG_HEADERS})
USES_EXAMPLES(${THIS})
USES_NVG(${THIS})
