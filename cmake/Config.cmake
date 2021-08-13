include_guard()

set(SHELL_PLATFORM ${CMAKE_SYSTEM_NAME})

set(BIG_ROOT ${CMAKE_CURRENT_LIST_DIR}/..)
set(BX_ROOT ${BIG_ROOT}/lib/bx)
set(BIMG_ROOT ${BIG_ROOT}/lib/bimg)
set(BGFX_ROOT ${BIG_ROOT}/lib/bgfx)
set(IMGUI_ROOT ${BIG_ROOT}/lib/imgui)

#TODO:PRIxPTR???
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
set(BGFX_COMPATIBILITY ${BX_ROOT}/include/compat/msvc)
endif()
