include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Standard.cmake)

function(USES_IMGUI THIS)
  USES_STD(${THIS})
  target_compile_definitions(${THIS} PRIVATE IMGUI_USER_CONFIG=<imgui/imconfig.h>)
  target_include_directories(${THIS} PRIVATE
    ${IMGUI_ROOT}
  )
  target_link_libraries(${THIS} PRIVATE ImGui)

endfunction()
