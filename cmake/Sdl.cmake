cmake_minimum_required(VERSION 3.14)

include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Standard.cmake)

function(USES_SDL THIS)
    USES_STD(${THIS})
    target_include_directories(${THIS} PRIVATE
    ${SDL_ROOT}/include
    )
    target_link_libraries(${THIS} SDL2main SDL2-static)
endfunction()

