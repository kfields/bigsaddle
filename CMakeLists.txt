cmake_minimum_required(VERSION 3.4)

include(./cmake/Config.cmake)
#
# SDL - Minimal build for now
#
set(SDL_SHARED OFF CACHE BOOL "Build a shared version of the library" FORCE)

set(SDL_RENDER OFF CACHE BOOL "Use render subsystem" FORCE)
set(SDL_AUDIO OFF CACHE BOOL "Use audio subsystem" FORCE)
set(SDL_HAPTIC OFF CACHE BOOL "Use haptic subsystem" FORCE)
set(HIDAPI OFF CACHE BOOL "Use HIDAPI for low level joystick drivers" FORCE)

set(VIDEO_X11 ${BIG_WM_X11} CACHE BOOL "Enable X11 support" FORCE)
set(VIDEO_WAYLAND ${BIG_WM_WAYLAND} CACHE BOOL "Enable Wayland support" FORCE)

add_subdirectory(lib/sdl)

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