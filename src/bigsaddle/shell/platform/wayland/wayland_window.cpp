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

void WaylandWindow::ReRender() {
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window_, &wmInfo);

    wl_surface* surface = wmInfo.info.wl.surface;
    wl_display* display = wmInfo.info.wl.display;
    //wl_surface_damage(surface, 0, 0, width(), height());
    //wl_surface_commit(surface);
}

} //namespace bigsaddle