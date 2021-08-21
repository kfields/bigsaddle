cmake_minimum_required(VERSION 3.15)

project(BigShell LANGUAGES CXX)

set(THIS ${PROJECT_NAME})

include(${BIG_ROOT}/cmake/Sdl.cmake)
include(${BIG_ROOT}/cmake/Bgfx.cmake)


set(SHELL_HEADERS
    dispatcher.h
    surface.h
    window_base.h
    window.h
)

set(SHELL_SRC
    dispatcher.cpp
    surface.cpp
    window_base.cpp
)

add_library(${THIS} ${SHELL_SRC} ${SHELL_HEADERS})
USES_SDL(${THIS})
USES_BGFX(${THIS})

target_link_libraries(${THIS} bgfx BigShell${BIG_PLATFORM})

if(${BIG_PLATFORM_WINDOWS})
    add_subdirectory(platform/windows)
elseif(${BIG_PLATFORM_LINUX})
    if(${BIG_WM_X11})
        add_subdirectory(platform/x11)
    elseif(${BIG_WM_WAYLAND})
        add_subdirectory(platform/wayland)
    endif()
endif()