#include "SDL.h"
#include "SDL_syswm.h"

#include "x11_window.h"


X11Window::~X11Window() {
}

Window X11Window::GetNativeHandle() {
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window_, &wmInfo);
    Window hwnd = wmInfo.info.x11.window;
    return hwnd;
}

void X11Window::ReDraw() {
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window_, &wmInfo);

    Window wnd = wmInfo.info.x11.window;
    Display* display = wmInfo.info.x11.display;
    XClearWindow(display, wnd);
}
