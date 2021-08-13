cmake_minimum_required(VERSION 3.15)

project(00-helloworld LANGUAGES CXX)

set(THIS ${PROJECT_NAME})

include(../../cmake/Big.cmake)

set(BIG_HEADERS
)

set(BIG_SRC
    main.cpp
)

add_executable(${THIS} ${BIG_SRC} ${BIG_HEADERS})
USES_BIG(${THIS})
