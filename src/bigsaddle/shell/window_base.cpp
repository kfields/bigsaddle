#include "SDL_syswm.h"

#include <imgui/imgui.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include <bigsaddle/gui/gui.h>

#include "window_base.h"

namespace bigsaddle {

std::map<uint32_t, WindowBase*> WindowBase::windowMap_;

WindowBase::WindowBase(WindowParams params) :
    windowId_(0),
    title_(params.title), 
    origin_(params.origin),
    size_(params.size),
    flags_(params.flags),
    window_(nullptr),
    gui_(nullptr)
{
}

WindowBase::~WindowBase() {
}

void WindowBase::Destroy() {
    UnmapWindow(windowId());
    SDL_DestroyWindow(window_);
    window_ = nullptr;
    Surface::Destroy();
}

void WindowBase::Create(WindowParams params) {
    window_ = SDL_CreateWindow(
      title_.c_str(), x(), y(), width(),
      height(), flags_);

    if (window_ == nullptr) {
        printf("WindowBase could not be created. SDL_Error: %s\n", SDL_GetError());
        return;
    }
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(window_, &wmi)) {
        printf(
            "SDL_SysWMinfo could not be retrieved. SDL_Error: %s\n",
            SDL_GetError());
        return;
    }
    SetWindowId(SDL_GetWindowID(window_));
    MapWindow(windowId(), this);
}

void WindowBase::Show() {
    SDL_ShowWindow(window_);
    Reset();
}

void WindowBase::SetPosition(Point origin) {
    origin_ = origin;
    SDL_SetWindowPosition(window_, origin.x, origin.y);
    Reset();
}

void WindowBase::SetSize(Size size) {
    SDL_SetWindowSize(window_, size.width, size.height);
    OnSize();
}

void WindowBase::OnSize() {
    SDL_GetWindowSize(window_, &size_.width, &size_.height);
    Reset();
}

bool WindowBase::Dispatch(const SDL_Event& event) {
    switch (event.type) {
        case SDL_QUIT:
            return false;
        case SDL_WINDOWEVENT:
            return DispatchWindowEvent(event);
        }
    return true;
}

bool WindowBase::DispatchWindowEvent(const SDL_Event& event) {
    if (event.window.windowID != windowId()) {
        WindowBase* child = windowMap_[event.window.windowID];
        if(child != nullptr)
            return windowMap_[event.window.windowID]->DispatchWindowEvent(event);
    }

    Uint8 window_event = event.window.event;
    switch (window_event) {
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            OnSize();
        case SDL_WINDOWEVENT_MOVED:
        case SDL_WINDOWEVENT_SHOWN:
            break;
        case SDL_WINDOWEVENT_EXPOSED:
            Render();
            break;
        case SDL_WINDOWEVENT_CLOSE:
            return false;
    }
    return true;
}

} //namespace bigsaddle