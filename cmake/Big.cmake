include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Shell.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/Gui.cmake)

function(USES_BIG THIS)
    USES_GUI(${THIS})
    USES_SHELL(${THIS})
    #target_compile_definitions(${THIS} PRIVATE IMGUI_USER_CONFIG=<imgui/imconfig.h>)
    #target_include_directories(${THIS} PRIVATE
    #  ${IMGUI_ROOT}
    #)
    target_link_libraries(${THIS} BigSaddle)

endfunction()
