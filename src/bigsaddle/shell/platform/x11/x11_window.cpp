#include <cstring>

#include "SDL.h"
#include "build_config/SDL_build_config.h"

#include "x11_window.h"

namespace bigsaddle {

X11Window::~X11Window() {
}

XWindow X11Window::GetNativeHandle() {
    XWindow hwnd = (XWindow)SDL_GetProperty(SDL_GetWindowProperties(window_), "SDL.window.x11.window", 0);
    return hwnd;
}

} //namespace bigsaddle