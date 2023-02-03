cmake_minimum_required(VERSION 3.14)

include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Standard.cmake)

function(USES_X11 THIS)
    USES_STD(${THIS})
    target_link_libraries(${THIS} PRIVATE X11)
endfunction()

