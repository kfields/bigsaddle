#include "SDL_syswm.h"

#include "win32_window.h"

namespace bigsaddle {

Win32Window::~Win32Window() {
}

RECT Win32Window::GetClientArea() {
  RECT frame;
  GetClientRect(GetNativeHandle(), &frame);
  return frame;
}

HWND Win32Window::GetNativeHandle() {
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window_, &wmInfo);
    HWND hwnd = wmInfo.info.win.window;
    return hwnd;
}

int Win32Window::AttachTo(HWND hParent) {
  Point origin(10, 10);
  Size size(1280, 720);

  CreateAndShow(WindowParams("NoName", origin, size));

  HWND hWnd = GetNativeHandle();
  SetParent(hWnd, hParent);
  const LONG nNewStyle = (GetWindowLong(hWnd, GWL_STYLE) & ~WS_POPUP & ~WS_OVERLAPPEDWINDOW) | WS_CHILDWINDOW | WS_VISIBLE;
  SetWindowLong(hWnd, GWL_STYLE, nNewStyle);
  const ULONG_PTR cNewStyle = GetClassLongPtr(hWnd, GCL_STYLE) | CS_DBLCLKS;
  SetClassLongPtr(hWnd, GCL_STYLE, cNewStyle);
  ShowWindow(hWnd, SW_NORMAL);

  return 0;
}

void Win32Window::ReRender() {
    HWND hwnd = GetNativeHandle();
    /*RECT rc;
    GetClientRect(hwnd, &rc);*/
    RECT rc = { 0, 0, 1, 1 };
    InvalidateRect(hwnd, &rc, false);
}

} //namespace bigsaddle