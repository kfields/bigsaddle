include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Standard.cmake)

function(USES_GLM THIS)
  USES_STD(${THIS})
  target_include_directories(${THIS} PRIVATE
    ${GLM_ROOT}
  )
  target_link_libraries(${THIS} glm)

endfunction()
