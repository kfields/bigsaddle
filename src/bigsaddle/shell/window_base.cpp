#include "SDL_syswm.h"

#include <imgui/imgui.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include <bigsaddle/gui/gui.h>

#include "window_base.h"

//TODO:Recycle handles
uint16_t WindowBase::instanceCount_ = 0;

bool WindowBase::Startup() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize. SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    return true;
}
void WindowBase::Shutdown() {
    bgfx::shutdown();
    SDL_Quit();
}

WindowBase::WindowBase(CreateParams& params) :
    title_(params.title), 
    origin_(params.origin),
    size_(params.size),
    flags_(params.flags),
    window_(nullptr),
    gui_(nullptr),
    viewId_(instanceCount_++)
{
}

WindowBase::~WindowBase() {
    Destroy();
}

bool WindowBase::DoCreate(CreateParams params) {
    window_ = SDL_CreateWindow(
      title_.c_str(), x(), y(), width(),
      height(), flags_);

    if (window_ == nullptr) {
        printf("WindowBase could not be created. SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(window_, &wmi)) {
        printf(
            "SDL_SysWMinfo could not be retrieved. SDL_Error: %s\n",
            SDL_GetError());
        return false;
    }
    windowId_ = SDL_GetWindowID(window_);

    return true;
}

bool WindowBase::Show() {
    SDL_ShowWindow(window_);
    Reset();
    return true;
}

void WindowBase::SetPosition(Point& origin) {
    origin_ = origin;
    SDL_SetWindowPosition(window_, origin.x, origin.y);
    Reset();
}

void WindowBase::SetSize(Size& size) {
    SDL_SetWindowSize(window_, size.width, size.height);
    OnSize();
}

void WindowBase::OnSize() {
    SDL_GetWindowSize(window_, &size_.width, &size_.height);
    Reset();
}

void WindowBase::Run() {
    for (bool quit = false; !quit;) {
        SDL_Event event;
        SDL_WaitEvent(&event);
        if (!Dispatch(event)) {
            quit = true;
            break;
        }
    }
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
    //TODO:  Will be fixed in next push
    /*if (event.window.windowID != windowId_) {
        WindowBase* child = childMap_[event.window.windowID];
        if(child != nullptr)
            return childMap_[event.window.windowID]->DispatchWindowEvent(event);
    }*/

    Uint8 window_event = event.window.event;
    switch (window_event) {
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            OnSize();
        case SDL_WINDOWEVENT_MOVED:
        case SDL_WINDOWEVENT_SHOWN:
        case SDL_WINDOWEVENT_EXPOSED:
            Draw();
            break;
    }
    return true;
}

void WindowBase::Destroy() {
    SDL_DestroyWindow(window_);
}
