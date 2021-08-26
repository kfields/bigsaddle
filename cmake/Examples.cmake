include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Big.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/Utils.cmake)

function(USES_EXAMPLES THIS)
    USES_BIG(${THIS})
    USES_UTILS(${THIS})
    target_include_directories(${THIS} PRIVATE
        ${BIG_ROOT}
    )
    target_link_libraries(${THIS} BigExamples)
    set_target_properties(
        ${THIS} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${BIG_ROOT}/assets")
endfunction()
