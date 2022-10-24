# bigsaddle :horse:

An opinionated, object oriented, minimalistic framework/starter kit for:

[bgfx](https://github.com/bkaradzic/bgfx)

[SDL](https://github.com/libsdl-org/SDL)

[Dear ImGui](https://github.com/ocornut/imgui)

:warning:  This is pre-alpha software.  Do not use it for production.

## Motivation

I wanted to take the pain out of cross platform 3D development for my personal projects

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
    cmake -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Debug ..

## Shader Compilation

* Build bgfx with the --with-tools option as described [here](https://bkaradzic.github.io/bgfx/build.html)
* shadercRelease needs to be on your path
* pass -DCOMPILE_SHADERS to cmake

## Wayland

    sudo apt install libwayland-dev
    sudo apt install libxkbcommon-dev
    sudo add-apt-repository ppa:christianrauch/libdecoration

    cmake -DBIG_WM_WAYLAND=ON -DCMAKE_BUILD_TYPE=Debug ..
