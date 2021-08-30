include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Standard.cmake)

function(USES_PUGI THIS)
  USES_STD(${THIS})
  target_include_directories(${THIS} PRIVATE
    ${PUGI_ROOT}/src
  )
  target_link_libraries(${THIS} pugixml)

endfunction()
