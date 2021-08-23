include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Standard.cmake)

function(USES_BGFX THIS)
    USES_STD(${THIS})
    target_include_directories(${THIS} PRIVATE
        ${BGFX_COMPATIBILITY}
        ${BX_ROOT}/include
        ${BX_ROOT}/3rdparty
        ${BIMG_ROOT}/include
        ${BIMG_ROOT}/3rdparty
        ${BGFX_ROOT}/include
        ${BGFX_ROOT}/3rdparty
        ${BGFX_ROOT}/3rdparty/khronos
    )
    target_link_libraries(${THIS} bgfx)
endfunction()
