include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/ImGui.cmake)

function(USES_GUI THIS)
  USES_IMGUI(${THIS})
  #target_compile_definitions(${THIS} PRIVATE IMGUI_USER_CONFIG=<imgui/imconfig.h>)
  #target_include_directories(${THIS} PRIVATE
  #  ${IMGUI_ROOT}
  #)
  target_link_libraries(${THIS} BigGui)

endfunction()
