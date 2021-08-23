cmake_minimum_required(VERSION 3.4)

project(NanoVG VERSION "0.1")

set(THIS ${PROJECT_NAME})

include(../../cmake/Bgfx.cmake)

set(NVG_HEADERS
    ${NVG_ROOT}/nanovg.h
)

set(_ ${NVG_ROOT})

set(NVG_SRC
    ${_}/nanovg.cpp
    ${_}/nanovg_bgfx.cpp
)

add_library(${THIS} STATIC
  ${NVG_SRC}
)
USES_BGFX(${THIS})