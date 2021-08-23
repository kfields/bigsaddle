cmake_minimum_required(VERSION 3.15)

project(Stb LANGUAGES CXX)

set(THIS ${PROJECT_NAME})

include(../../cmake/Bgfx.cmake)

set(STB_HEADERS
)

set(STB_SRC
    stb.cpp
)

add_library(${THIS} ${STB_SRC} ${STB_HEADERS})
USES_BGFX(${THIS})
