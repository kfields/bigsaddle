#include "build_config/SDL_build_config.h"

#include <imgui/imgui.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include <bigsaddle/gui/gui.h>

#include "window_base.h"

namespace bigsaddle {

std::map<uint32_t, WindowBase*> WindowBase::windowMap_;

WindowBase::WindowBase(WindowParams params) :
    windowId_(0),
    name_(params.name), 
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

void WindowBase::Create() {
    window_ = SDL_CreateWindow(
      name_.c_str(), width(),
      height(), flags_);

    if (window_ == nullptr) {
        printf("WindowBase could not be created. SDL_Error: %s\n", SDL_GetError());
        return;
    }
    SetWindowId(SDL_GetWindowID(window_));
    MapWindow(windowId(), this);
    ComputePixelSize();
}

void WindowBase::Show() {
    SDL_ShowWindow(window_);
    Reset();
}

void WindowBase::SetPosition(Point origin) {
    origin_ = origin;
    SDL_SetWindowPosition(window_, origin.x, origin.y);
}

void WindowBase::SetSize(Size size) {
    SDL_SetWindowSize(window_, size.width, size.height);
    OnSize();
}

void WindowBase::ComputePixelSize() {
    int w, h;
    SDL_GetWindowSize(window_, &w, &h);
    size_ = Size(w, h);
    // printf("Window size: width=%i, height=%i\n", w, h);
    int pw, ph;
    SDL_GetWindowSizeInPixels(window_, &pw, &ph);
    pixelSize_ = Size(pw, ph);
    // printf("Window size in pixels: width=%i, height=%i\n", pw, ph);
    pixelRatio_ = (float)pw / (float)w;
    // printf("Pixel ratio: %f\n", pixelRatio_);
}

void WindowBase::OnSize() {
    ComputePixelSize();
    Reset();
}

bool WindowBase::Dispatch(const SDL_Event& event) {
    if (event.type >= SDL_EVENT_WINDOW_FIRST && event.type <= SDL_EVENT_WINDOW_LAST)
        return DispatchWindowEvent(event);

    switch (event.type) {
        case SDL_EVENT_QUIT:
            return false;
        }
    return true;
}

bool WindowBase::DispatchWindowEvent(const SDL_Event& event) {
    if (event.window.windowID != windowId()) {
        WindowBase* child = windowMap_[event.window.windowID];
        if(child != nullptr)
            return windowMap_[event.window.windowID]->DispatchWindowEvent(event);
    }

    switch (event.type) {
        case SDL_EVENT_WINDOW_RESIZED:
            OnSize();
        case SDL_EVENT_WINDOW_MOVED:
        case SDL_EVENT_WINDOW_SHOWN:
            break;
        case SDL_EVENT_WINDOW_EXPOSED:
            Render();
            break;
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            return false;
    }
    return true;
}

} //namespace bigsaddle