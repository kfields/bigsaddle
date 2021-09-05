include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Standard.cmake)

function(USES_ENTT THIS)
  USES_STD(${THIS})
  target_include_directories(${THIS} PRIVATE
    ${ENTT_ROOT/src}
  )
  target_link_libraries(${THIS} EnTT)

endfunction()
