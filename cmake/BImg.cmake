include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Standard.cmake)

function(USES_BIMG THIS)
    USES_STD(${THIS})

    target_include_directories(${THIS} PRIVATE
        ${BX_COMPATIBILITY}
        ${BIMG_ROOT}/include
        ${BIMG_ROOT}/3rdparty
    )
    target_link_libraries(${THIS} bimg)
endfunction()