# bigsaddle :horse:

An opinionated, object oriented, minimalistic framework/starter kit for:

[bgfx](https://github.com/bkaradzic/bgfx)

[SDL](https://github.com/libsdl-org/SDL)

[Dear ImGui](https://github.com/ocornut/imgui)

:warning:  This is pre-alpha software.  Do not use it for production.

## Motivation

I wanted to take the pain out of cross platform development for my personal projects

## Features

Uses ImGui docking branch with support for docking and viewports

## Quick Start

    git clone --recursive https://github.com/kfields/bigsaddle
    cd bigsaddle
    mkdir build
    cd build
    cmake ..

### Or

    mkdir build-debug
    cd build-debug
    cmake -DCMAKE_BUILD_TYPE=Debug ..

## Wayland

    sudo apt install libwayland-dev
    sudo apt install libxkbcommon-dev