include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Sdl.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/Bgfx.cmake)

if(${BIG_PLATFORM_WINDOWS})
    set(SHELL_HEADER "<bigsaddle/shell/platform/windows/win32_window.h>")
    set(SHELL_CLASS "Win32Window")
elseif(${BIG_PLATFORM_LINUX})
    if(${BIG_WM_X11})
        set(SHELL_HEADER "<bigsaddle/shell/platform/x11/x11_window.h>")
        set(SHELL_CLASS "X11Window")
    elseif(${BIG_WM_WAYLAND})
        set(SHELL_HEADER "<bigsaddle/shell/platform/wayland/wayland_window.h>")
        set(SHELL_CLASS "WaylandWindow")
    endif()
endif()

function(USES_SHELL THIS)
    USES_SDL(${THIS})
    USES_BGFX(${THIS})

    target_compile_definitions(${THIS} PRIVATE SHELL_HEADER=${SHELL_HEADER} SHELL_CLASS=${SHELL_CLASS})
    target_link_libraries(${THIS} PRIVATE BigShell BigShell${CMAKE_SYSTEM_NAME})

endfunction()
