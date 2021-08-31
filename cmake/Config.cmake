include_guard()

include(CMakeDependentOption)

set(BIG_PLATFORM ${CMAKE_SYSTEM_NAME})

set(BIG_PLATFORM_WINDOWS OFF)
set(BIG_PLATFORM_LINUX OFF)

if(${BIG_PLATFORM} STREQUAL "Windows")
    set(BIG_PLATFORM_WINDOWS ON)
elseif(${BIG_PLATFORM} STREQUAL "Linux")
    set(BIG_PLATFORM_LINUX ON)
endif()

option(BIG_RENDERER_GL "Use GL" OFF)
cmake_dependent_option(BIG_RENDERER_VULKAN "Use Vulkan" ON
                       "BIG_PLATFORM_LINUX; NOT BIG_RENDERER_GL" OFF)

option(BIG_WM_WAYLAND "Use Wayland" OFF)
cmake_dependent_option(BIG_WM_X11 "Use X11" ON
                        "BIG_PLATFORM_LINUX; NOT BIG_WM_WAYLAND" OFF)
                       
if(${BIG_WM_X11})
    set(BIG_COMPILE_DEFS
        SDL_VIDEO_DRIVER_X11=1
    )
elseif(${BIG_WM_WAYLAND})
    set(BIG_COMPILE_DEFS
        SDL_VIDEO_DRIVER_WAYLAND=1
    )
endif()

if(${BIG_RENDERER_GL})
    set(BIG_COMPILE_DEFS ${BIG_COMPILE_DEFS}
        BGFX_CONFIG_RENDERER_OPENGL=1
        BGFX_CONFIG_RENDERER_OPENGLES=0
        BGFX_CONFIG_RENDERER_VULKAN=0
    )
elseif(${BIG_RENDERER_VULKAN})
    set(BIG_COMPILE_DEFS ${BIG_COMPILE_DEFS}
        BGFX_CONFIG_RENDERER_OPENGL=0
        BGFX_CONFIG_RENDERER_OPENGLES=0
        BGFX_CONFIG_RENDERER_VULKAN=1
    )
endif()

set(BIG_ROOT ${CMAKE_CURRENT_LIST_DIR}/..)
set(FMT_ROOT ${BIG_ROOT}/lib/fmt)
set(GLM_ROOT ${BIG_ROOT}/lib/glm)
set(PUGI_ROOT ${BIG_ROOT}/lib/pugixml)
set(IMGUI_ROOT ${BIG_ROOT}/lib/imgui)
set(BX_ROOT ${BIG_ROOT}/lib/bx)
set(BIMG_ROOT ${BIG_ROOT}/lib/bimg)
set(BGFX_ROOT ${BIG_ROOT}/lib/bgfx)
set(NVG_ROOT ${BIG_ROOT}/lib/bgfx/examples/common/nanovg)
set(EXAMPLES_ROOT ${BIG_ROOT}/examples)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
set(BX_COMPATIBILITY ${BX_ROOT}/include/compat/msvc)
endif()
