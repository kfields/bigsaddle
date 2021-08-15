#include "SDL.h"
#include "SDL_syswm.h"

#include "x11_window.h"

namespace bigsaddle {

X11Window::~X11Window() {
}

XWindow X11Window::GetNativeHandle() {
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window_, &wmInfo);
    XWindow hwnd = wmInfo.info.x11.window;
    return hwnd;
}

void X11Window::ReRender() {
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window_, &wmInfo);

    XID wnd = wmInfo.info.x11.window;
    Display* display = wmInfo.info.x11.display;
    XClearArea(display, wnd, 0, 0, 1, 1, true);
}

} //namespace bigsaddle