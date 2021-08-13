include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Standard.cmake)

function(USES_BGFX THIS)
    USES_STD(${THIS})
    target_include_directories(${THIS} PRIVATE
        ${BX_ROOT}/include/compat/msvc
        ${BX_ROOT}/include
        ${BX_ROOT}/3rdparty
        ${BIMG_ROOT}/include
        ${BIMG_ROOT}/3rdparty
        #${BIMG_ROOT}/3rdparty/astc-codec/include
        #${BIMG_ROOT}/3rdparty/astc-codec
        ${BGFX_ROOT}/include
        ${BGFX_ROOT}/3rdparty
        ${BGFX_ROOT}/3rdparty/khronos
        #${BGFX_ROOT}/3rdparty/dxsdk/include
    )
    target_link_libraries(${THIS} bgfx)
endfunction()
