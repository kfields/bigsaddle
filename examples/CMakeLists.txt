cmake_minimum_required(VERSION 3.15)

project(BigExamples LANGUAGES CXX)

set(THIS ${PROJECT_NAME})

include(${BIG_ROOT}/cmake/Big.cmake)
include(${BIG_ROOT}/cmake/Utils.cmake)
include(${BIG_ROOT}/cmake/Entt.cmake)

set(HEADERS
    common.h
    example_app.h
    common/color.h
    common/texture.h
    common/sprite.h
    common/tile.h
    common/csv.h
    common/timer.h
)

set(SOURCES
    example_app.cpp
    example_dialog.cpp
    common/entity.cpp
)

add_library(${THIS} ${HEADERS} ${SOURCES})
USES_BIG(${THIS})
USES_UTILS(${THIS})
USES_ENTT(${THIS})

add_subdirectory(00-helloworld)
add_subdirectory(01-cubes)
add_subdirectory(02-metaballs)
add_subdirectory(03-raymarch)
add_subdirectory(04-mesh)
add_subdirectory(20-nanovg)
add_subdirectory(50-quad)
add_subdirectory(51-sprite)
add_subdirectory(52-spritesheet)
add_subdirectory(53-animatedsprite)
add_subdirectory(54-tileset)
add_subdirectory(55-tilemap)