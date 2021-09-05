include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Big.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/Utils.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/Glm.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/Entt.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/Fmt.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/Pugi.cmake)

include(${CMAKE_CURRENT_LIST_DIR}/bgfx/shader.cmake)

function(USES_EXAMPLES THIS)
    USES_BIG(${THIS})
    USES_UTILS(${THIS})
    USES_GLM(${THIS})
    USES_ENTT(${THIS})
    USES_FMT(${THIS})
    USES_PUGI(${THIS})

    target_include_directories(${THIS} PRIVATE
        ${BIG_ROOT}
    )
    target_link_libraries(${THIS} BigExamples)
    set_target_properties(
        ${THIS} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${BIG_ROOT}/examples/runtime")
endfunction()

function(add_example THIS)
    add_executable(${THIS} ${ARGN})
    USES_EXAMPLES(${THIS})
    if(${COMPILE_SHADERS})
        file( GLOB SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.sc )
        foreach( SHADER ${SHADERS} )
            add_bgfx_shader( ${THIS} ${SHADER} )
        endforeach()
        source_group( "Shader Files" FILES ${SHADERS})
    endif()
endfunction()