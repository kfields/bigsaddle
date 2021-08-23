include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Stb.cmake)

function(USES_NVG THIS)
  USES_STB(${THIS})
  target_include_directories(${THIS} PRIVATE
    ${NVG_ROOT}
  )
  target_link_libraries(${THIS} NanoVG)

endfunction()
