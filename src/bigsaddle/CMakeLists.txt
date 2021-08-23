cmake_minimum_required(VERSION 3.15)

project(BigSaddle LANGUAGES CXX)

set(THIS ${PROJECT_NAME})

include(../../cmake/Shell.cmake)
include(../../cmake/Gui.cmake)

set(BIG_HEADERS
    app.h
)

set(BIG_SRC
    app.cpp
    stb.cpp
)

add_library(${THIS} ${BIG_SRC} ${BIG_HEADERS})
USES_SHELL(${THIS})
USES_GUI(${THIS})

add_subdirectory(gui)
add_subdirectory(shell)
