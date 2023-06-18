cmake_minimum_required(VERSION 3.4)

project(BIGSADDLE VERSION 0.1.0 LANGUAGES CXX C)

include(./cmake/Config.cmake)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(-DCMAKE_CXX_STANDARD=20 -std=c++20)

#
# fmt
#
add_subdirectory(lib/fmt)

#
# pugixml
#
add_subdirectory(lib/pugixml)

#
# GLM
#
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared library" FORCE)
set(BUILD_STATIC_LIBS ON CACHE BOOL "Build static library" FORCE)

add_subdirectory(lib/glm)

#
# Entt
#
add_subdirectory(lib/entt)

#
# SDL - Minimal build for now
#
set(HAVE_LIBC ON)
set(SDL_SHARED OFF CACHE BOOL "Build a shared version of the library" FORCE)
set(SDL_STATIC ON CACHE BOOL "Build a static version of the library" FORCE)
set(SDL_TEST OFF CACHE BOOL "Build the SDL3_test library" FORCE)

set(SDL_RENDER OFF CACHE BOOL "Use render subsystem" FORCE)
set(SDL_AUDIO OFF CACHE BOOL "Use audio subsystem" FORCE)
set(SDL_HAPTIC OFF CACHE BOOL "Use haptic subsystem" FORCE)
set(HIDAPI OFF CACHE BOOL "Use HIDAPI for low level joystick drivers" FORCE)

set(VIDEO_X11 ${BIG_WM_X11} CACHE BOOL "Enable X11 support" FORCE)
set(VIDEO_WAYLAND ${BIG_WM_WAYLAND} CACHE BOOL "Enable Wayland support" FORCE)

add_subdirectory(lib/sdl)

#
# bimg
#
add_subdirectory(lib/astc-encoder)
add_subdirectory(src/bimg)

#
# bgfx
#
add_subdirectory(src/bgfx)

#
# NanoVG
#
add_subdirectory(src/nanovg)

#
#ImGui
#
add_subdirectory(src/imgui)

#
#BigSaddle
#
add_subdirectory(src/bigsaddle)

#
#Examples
#
add_subdirectory(examples)