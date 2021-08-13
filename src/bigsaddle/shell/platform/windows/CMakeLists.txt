cmake_minimum_required(VERSION 3.14)

set(THIS ${PROJECT_NAME}${CMAKE_SYSTEM_NAME})

include(${BIG_ROOT}/cmake/Sdl.cmake)
include(${BIG_ROOT}/cmake/Gui.cmake)
include(${BIG_ROOT}/cmake/Bgfx.cmake)

file(GLOB SHELL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB SHELL_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${THIS} STATIC
  ${SHELL_SRC}
  ${SHELL_INCLUDE}
)
USES_SDL(${THIS})
USES_GUI(${THIS})
USES_BGFX(${THIS})

