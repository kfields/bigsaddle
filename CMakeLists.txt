cmake_minimum_required(VERSION 3.4)

#
# SDL
#
#set(SDL_VIDEO OFF CACHE BOOL "Use video subsystem" FORCE)
set(SDL_RENDER OFF CACHE BOOL "Use render subsystem" FORCE)
set(SDL_AUDIO OFF CACHE BOOL "Use audio subsystem" FORCE)
set(SDL_HAPTIC OFF CACHE BOOL "Use audio subsystem" FORCE)

set(VIDEO_X11 ON CACHE BOOL "Enable X11 support" FORCE)
set(VIDEO_WAYLAND OFF CACHE BOOL "Enable Wayland support" FORCE)
set(WAYLAND_SHARED OFF CACHE BOOL "Enable Wayland support" FORCE)
set(VIDEO_VULKAN ON CACHE BOOL "Enable Vulkan support" FORCE)

#set(VIDEO_DUMMY OFF CACHE BOOL "Use dummy video driver" FORCE)
set(VIDEO_OPENGL OFF CACHE BOOL "Include OpenGL support" FORCE)
set(VIDEO_OPENGLES OFF CACHE BOOL "Include OpenGL support" FORCE)
#set(DIRECTX OFF CACHE BOOL "Use DirectX for Windows audio/video" FORCE)
#set(DISKAUDIO OFF CACHE BOOL "Support the disk writer audio driver" FORCE)
#set(DUMMYAUDIO OFF CACHE BOOL "Support the dummy audio driver" FORCE)
#set(RENDER_D3D OFF CACHE BOOL "Enable the Direct3D render driver" FORCE)
#set(VIDEO_VULKAN OFF CACHE BOOL "Enable Vulkan support" FORCE)
set(SDL_SHARED OFF CACHE BOOL "Build a shared version of the library" FORCE)
set(HIDAPI OFF CACHE BOOL "Use HIDAPI for low level joystick drivers" FORCE)

add_subdirectory(lib/sdl)

#
# bgfx
#
add_subdirectory(src/bgfx)

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