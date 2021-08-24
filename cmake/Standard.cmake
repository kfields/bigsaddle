cmake_minimum_required(VERSION 3.14)

include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Config.cmake)

function(USES_STD THIS)
    if(${BIG_PLATFORM} STREQUAL "Linux")
        target_compile_definitions(${THIS} PRIVATE
            ${BIG_COMPILE_DEFS}
            NOMINMAX
        )
    endif()
    target_compile_features(${THIS} PUBLIC cxx_std_17)
    #target_compile_features(${THIS} PUBLIC cxx_std_20)
    target_include_directories(${THIS} PRIVATE
        ${BIG_ROOT}/src
        ${BIG_ROOT}/lib
    )
endfunction()

