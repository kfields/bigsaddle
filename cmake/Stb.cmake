include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Standard.cmake)

function(USES_STB THIS)
  USES_STD(${THIS})
  #target_include_directories(${THIS} PRIVATE
  #  ${NVG_ROOT}
  #)
  target_link_libraries(${THIS} Stb)

endfunction()
