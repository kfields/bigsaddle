#include <cstring>

#include "SDL.h"
#include "SDL_syswm.h"
#include "build_config/SDL_build_config.h"

#include "x11_window.h"

namespace bigsaddle {

X11Window::~X11Window() {
}

XWindow X11Window::GetNativeHandle() {
    SDL_SysWMinfo wmInfo;
    SDL_GetWindowWMInfo(window_, &wmInfo, SDL_SYSWM_CURRENT_VERSION);
    XWindow hwnd = wmInfo.info.x11.window;
    return hwnd;
}

} //namespace bigsaddle