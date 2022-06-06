cmake_minimum_required(VERSION 3.14)

include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Config.cmake)

function(USES_STD THIS)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_definitions(${THIS} PRIVATE BX_CONFIG_DEBUG=1)
    else()
        target_compile_definitions(${THIS} PRIVATE BX_CONFIG_DEBUG=0)
    endif()

    if(${BIG_PLATFORM} STREQUAL "Linux")
        target_compile_definitions(${THIS} PRIVATE
            ${BIG_COMPILE_DEFS}
            NOMINMAX
        )
    endif()
    #target_compile_features(${THIS} PUBLIC cxx_std_17)
    target_compile_features(${THIS} PUBLIC cxx_std_20)
    set_property(TARGET ${THIS} PROPERTY CXX_STANDARD 20)
    if ((MSVC) AND (MSVC_VERSION GREATER_EQUAL 1914))
        target_compile_options(${THIS} PRIVATE "/Zc:__cplusplus")
    endif()
    target_include_directories(${THIS} PRIVATE
        ${BIG_ROOT}/src
        ${BIG_ROOT}/lib
    )
endfunction()

