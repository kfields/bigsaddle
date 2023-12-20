cmake_minimum_required(VERSION 3.15)

set(THIS ImGui)

project(${THIS} LANGUAGES CXX)

include(../../cmake/Standard.cmake)

set(IMGUI_HEADERS
    ${IMGUI_ROOT}/imconfig.h
    ${IMGUI_ROOT}/imgui_internal.h
    ${IMGUI_ROOT}/imgui.h
    ${IMGUI_ROOT}/imstb_rectpack.h
    ${IMGUI_ROOT}/imstb_textedit.h
    ${IMGUI_ROOT}/imstb_truetype.h
)

set(IMGUI_SRC
    ${IMGUI_ROOT}/imgui_demo.cpp
    ${IMGUI_ROOT}/imgui_draw.cpp
    ${IMGUI_ROOT}/imgui_widgets.cpp
    ${IMGUI_ROOT}/imgui_tables.cpp
    ${IMGUI_ROOT}/imgui.cpp
)

add_library(${THIS} STATIC ${IMGUI_SRC} ${IMGUI_HEADERS})
USES_STD(${THIS})
target_include_directories(${THIS} PRIVATE ${IMGUI_ROOT})
