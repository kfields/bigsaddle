cmake_minimum_required(VERSION 3.4)

project(bgfx VERSION "0.1")

set(THIS ${PROJECT_NAME})

include(../../cmake/Standard.cmake)

set(BX_SRC
  ${BX_ROOT}/src/amalgamated.cpp
)

set(OPTION_ASTC_TESTS OFF CACHE BOOL "Build all the unit tests" FORCE)

add_subdirectory(${BIMG_ROOT}/3rdparty/astc-codec astc-codec)

set(BIMG_SRC
  ${BIMG_ROOT}/src/image.cpp
  ${BIMG_ROOT}/src/image_gnf.cpp
)

set(BGFX_HEADERS
    ${BGFX_ROOT}/include/bgfx/bgfx.h
)

set(BGFX_SRC
    ${BGFX_ROOT}/src/amalgamated.cpp
)

add_library(${THIS} STATIC
  ${BX_SRC}
  ${BX_ROOT}/include
  ${BIMG_SRC}
  ${BIMG_ROOT}/include
  ${BGFX_SRC}
  ${BGFX_ROOT}/include
)
USES_STD(${THIS})

#TODO:PRIxPTR???
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
target_compile_definitions(${THIS} PRIVATE PRIxPTR="Ix")
endif()

install(TARGETS ${THIS} EXPORT BgfxTargets
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# add include files
target_sources(${THIS} PRIVATE ${AIMGFX_INCLUDE}) # for intellisense
target_include_directories(${THIS}
	PUBLIC
		$<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${BGFX_ROOT}>
	PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
            ${BX_ROOT}/include/compat/msvc
        endif()
        ${BX_ROOT}/include
        ${BX_ROOT}/3rdparty
        ${BIMG_ROOT}/include
        ${BIMG_ROOT}/3rdparty
        ${BGFX_ROOT}/include
        ${BGFX_ROOT}/3rdparty
        ${BGFX_ROOT}/3rdparty/khronos
        ${BGFX_ROOT}/3rdparty/dxsdk/include
)

target_link_libraries(${THIS} PRIVATE astc-codec)
