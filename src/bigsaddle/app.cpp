#include <string>
#include <iostream>

#include "SDL.h"
#include "SDL_syswm.h"

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/math.h>
#include <imgui.h>

#include "app.h"
#include "gui/gui.h"

namespace bigsaddle {

static void setup_bgfx_platform_data(bgfx::PlatformData &pd, const SDL_SysWMinfo &wmi);

App::~App() {
    paint_thread_.detach();
}

void App::DoCreate(CreateParams params) {
    Window::DoCreate(params);
    auto result = bgfx::renderFrame(); // single threaded mode

    bgfx::PlatformData pd{};
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window_, &wmInfo);

    setup_bgfx_platform_data(pd, wmInfo);
    //pd.nwh = GetHandle();

    bgfx::Init bgfx_init;
    bgfx_init.type = bgfx::RendererType::Count; // auto choose renderer
    bgfx_init.resolution.width = width();
    bgfx_init.resolution.height = height();
    bgfx_init.resolution.reset = resetFlags_;
    bgfx_init.platformData = pd;
    bgfx::init(bgfx_init);

    bgfx::setViewClear(
        viewId(), BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x11111111, 1.0f, 0);
    bgfx::setViewRect(viewId(), 0, 0, width(), height());


    CreateGui(params);
}

void App::PostCreate(WindowBase::CreateParams params) {
    paint_thread_ = std::thread([this]() {
        while (state_ == State::kRunning) {
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
            ReDraw();
        }
    });

    Window::PostCreate(params);
}

bool App::CreateGui(CreateParams params){
    gui_ = new Gui(*this);
    return gui_->Create();
}

void App::Reset()
{
    bgfx::reset(width(), height(), resetFlags_);
}

bool App::Dispatch(const SDL_Event& event) {
    //if (gui().Dispatch(&event)) return true;
    gui().Dispatch(&event);
    return Window::Dispatch(event);
}

void App::Destroy() {
    gui().Destroy(); //TODO: Have gui call this in it's destructor
    Window::Destroy();
    bgfx::shutdown();
    SDL_Quit();
}

void App::PreDraw() {
    gui().NewFrame();
    Window::PreDraw();
}

void App::DoDraw() {
    Window::DoDraw();
    bgfx::touch(viewId());
}

void App::PostDraw() {
    gui().Render();
    bgfx::frame();
    Window::PostDraw();
}

int App::Run() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize. SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    Create();

    while (state_ == State::kRunning) {
        SDL_Event event;
        SDL_WaitEvent(&event);
        if (!Dispatch(event)) {
            state_ = State::kShutdown;
        }
    }

    return 0;
}

static void setup_bgfx_platform_data(bgfx::PlatformData &pd, const SDL_SysWMinfo &wmi) {
    switch (wmi.subsystem) {
        case SDL_SYSWM_UNKNOWN: abort();

#if defined(SDL_VIDEO_DRIVER_X11)
        case SDL_SYSWM_X11:
            pd.ndt = wmi.info.x11.display;
            pd.nwh = (void *)(uintptr_t)wmi.info.x11.window;
            break;
#endif

#if defined(SDL_VIDEO_DRIVER_WAYLAND)
        case SDL_SYSWM_WAYLAND:
            pd.ndt = wmi.info.wl.display;
            pd.nwh = wmi.info.wl.surface;
            break;
#endif

#if defined(SDL_VIDEO_DRIVER_MIR)
        case SDL_SYSWM_MIR:
            pd.ndt = wmi.info.mir.connection;
            pd.nwh = wmi.info.mir.surface;
            break;
#endif

#if defined(SDL_VIDEO_DRIVER_COCOA)
        case SDL_SYSWM_COCOA:
            pd.ndt = NULL;
            pd.nwh = wmi.info.cocoa.window;
            break;
#endif

#if defined(SDL_VIDEO_DRIVER_UIKIT)
        case SDL_SYSWM_UIKIT:
            pd.ndt = NULL;
            pd.nwh = wmi.info.uikit.window;
            break;
#endif

#if defined(SDL_VIDEO_DRIVER_WINDOWS)
        case SDL_SYSWM_WINDOWS:
            pd.ndt = NULL;
            pd.nwh = wmi.info.win.window;
            break;
#endif

#if defined(SDL_VIDEO_DRIVER_WINRT)
        case SDL_SYSWM_WINRT:
            pd.ndt = NULL;
            pd.nwh = wmi.info.winrt.window;
            break;
#endif

#if defined(SDL_VIDEO_DRIVER_VIVANTE)
        case SDL_SYSWM_VIVANTE:
            pd.ndt = wmi.info.vivante.display;
            pd.nwh = wmi.info.vivante.window;
            break;
#endif

        //default: spdlog::critical("Unknown Window system."); std::abort();
    }
    pd.context = NULL;
    pd.backBuffer = NULL;
    pd.backBufferDS = NULL;
}

} //namespace bigsaddle