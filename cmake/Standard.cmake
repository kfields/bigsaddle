cmake_minimum_required(VERSION 3.14)

include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Config.cmake)

function(USES_STD THIS)
    if(${SHELL_PLATFORM} STREQUAL "Linux")
        target_compile_definitions(${THIS} PRIVATE
        SDL_VIDEO_DRIVER_X11=1
        SDL_VIDEO_VULKAN=1
        BGFX_CONFIG_RENDERER_VULKAN=1
        BGFX_CONFIG_RENDERER_OPENGL=0
        BGFX_CONFIG_RENDERER_OPENGLES=0
        )
        #target_compile_definitions(${THIS} PRIVATE SDL_VIDEO_DRIVER_WAYLAND)
    endif()
    target_compile_features(${THIS} PUBLIC cxx_std_17)
    #target_compile_features(${THIS} PUBLIC cxx_std_20)
    target_include_directories(${THIS} PRIVATE
        ${BIG_ROOT}/src
        ${BIG_ROOT}/lib
    )
endfunction()

