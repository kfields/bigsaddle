cmake_minimum_required(VERSION 3.14)

include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Standard.cmake)

function(USES_GL THIS)
    USES_STD(${THIS})
    target_link_libraries(${THIS} GL)
endfunction()

