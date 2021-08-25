cmake_minimum_required(VERSION 3.15)

project(BigGui LANGUAGES CXX)

set(THIS ${PROJECT_NAME})

include(${BIG_ROOT}/cmake/Sdl.cmake)
include(${BIG_ROOT}/cmake/ImGui.cmake)
include(${BIG_ROOT}/cmake/Bgfx.cmake)
include(${BIG_ROOT}/cmake/Shell.cmake)


set(SHELL_HEADERS
    gui.h
    gui_font.h
    gui_renderer.h
    gui_viewport.h
    gui_user.h
)

set(SHELL_SRC
    gui.cpp
    gui_font.cpp
    gui_hooks.cpp
    gui_renderer.cpp
    gui_viewport.cpp
    gui_user.cpp
)

add_library(${THIS} ${SHELL_SRC} ${SHELL_HEADERS})
USES_SDL(${THIS})
USES_IMGUI(${THIS})
USES_BGFX(${THIS})
USES_SHELL(${THIS})

target_include_directories(${THIS} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/renderer)

target_link_libraries(${THIS} ImGui bgfx BigShell${BIG_PLATFORM})
