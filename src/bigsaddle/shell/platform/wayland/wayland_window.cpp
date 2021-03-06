#include "SDL.h"
#include "SDL_syswm.h"

#include <wayland-client.h>

#include "wayland_window.h"

namespace bigsaddle {

WaylandWindow::~WaylandWindow() {
}

wl_surface* WaylandWindow::GetNativeHandle() {
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window_, &wmInfo);
    wl_surface* hwnd = wmInfo.info.wl.surface;
    return hwnd;
}

} //namespace bigsaddle