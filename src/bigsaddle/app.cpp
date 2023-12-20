#include <string>
#include <iostream>

#include "SDL.h"
#include "build_config/SDL_build_config.h"

#include <imgui/imgui.h>

#include <bx/math.h>
#include <bx/timer.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include "app.h"
#include "gui/gui.h"

namespace bigsaddle {

static void SetupBgfxPlatformData(SDL_Window *sdl_window_, bgfx::PlatformData &pd);

App::App(WindowParams params) : Window(params),
    state_(State::kRunning),
    resetFlags_(BGFX_RESET_VSYNC),
    debugFlags_(BGFX_DEBUG_TEXT),
    timeOffset_(bx::getHPCounter())
    {}

App::~App() {
}

void App::Destroy() {
    bgfx::frame();
    delete gui_;
    bgfx::shutdown();
    Window::Destroy();
}

void App::Create() {
    Window::Create();

    CreateGfx();

    CreateGui();
}

void App::CreateGfx() {
    auto result = bgfx::renderFrame(); // single threaded mode

    bgfx::PlatformData pd{};

    SetupBgfxPlatformData(window_, pd);

    bgfx::Init bgfx_init;
    bgfx_init.debug = debug_;
    bgfx_init.type = bgfx::RendererType::Count; // auto choose renderer
    bgfx_init.resolution.width = width();
    //bgfx_init.resolution.width = pixelWidth();
    bgfx_init.resolution.height = height();
    //bgfx_init.resolution.height = pixelHeight();
    bgfx_init.resolution.reset = resetFlags_;
    bgfx_init.platformData = pd;
    bgfx::init(bgfx_init);

    bgfx::setViewClear(0
        , BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH
        , 0x303030ff
        , 1.0f
        , 0
    );

    bgfx::setViewRect(viewId(), 0, 0, width(), height());
    //bgfx::setViewRect(viewId(), 0, 0, pixelWidth(), pixelHeight());
}

void App::CreateGui(){
    gui_ = &Gui::Produce(*this);
}

void App::Reset(ResetKind kind)
{
    bgfx::reset(width(), height(), resetFlags_);
    //bgfx::reset(pixelWidth(), pixelHeight(), resetFlags_);
    bgfx::setViewRect(viewId(), 0, 0, width(), height());
    //bgfx::setViewRect(viewId(), 0, 0, pixelWidth(), pixelHeight());
    for (auto child : children_) {
        child->Reset(ResetKind::kSoft);
    }
}

bool App::Dispatch(const SDL_Event& event) {
    if (state_ == State::kShutdown)
        return false;
    //if (gui().Dispatch(&event)) return true;
    gui().Dispatch(&event);
    capture_ = false;
    switch (event.type)
    {
        case SDL_EVENT_KEY_DOWN:
        {
            int key = event.key.keysym.scancode;
            if (key == SDL_SCANCODE_F11) {
                capture_ = true;
                return true;
            }
        }
    }
    return Window::Dispatch(event);
}

void App::PreRender() {
    Window::PreRender();
    gui().NewFrame();
}

void App::PostRender() {
    gui().Render();
    bgfx::touch(viewId());
    bgfx::frame(capture_);
    Window::PostRender();
}

int App::Run() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize. SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    CreateAndShow();

    const int FPS = 60;
    const int frameDelay = 1000 / FPS;
    Uint32 frameStart;
    int frameTime;

    while (state_ == State::kRunning) {
        frameStart = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (!Dispatch(event)) {
                state_ = State::kShutdown;
            }
        }

        Render();

        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }

    Destroy();

    SDL_Quit();

    return 0;
}

static void SetupBgfxPlatformData(SDL_Window *sdl_window_, bgfx::PlatformData &pd) {
#if defined(SDL_VIDEO_DRIVER_X11)
    //pd.ndt = wmi.info.x11.display;
    pd.ndt = SDL_GetProperty(SDL_GetWindowProperties(sdl_window_), "SDL.window.x11.display", 0);
    //pd.nwh = (void *)(uintptr_t)wmi.info.x11.window;
    pd.nwh = SDL_GetProperty(SDL_GetWindowProperties(sdl_window_), "SDL.window.x11.window", 0);
#endif

#if defined(SDL_VIDEO_DRIVER_WAYLAND)
    pd.ndt = wmi.info.wl.display;
    pd.nwh = wmi.info.wl.surface;
#endif

#if defined(SDL_VIDEO_DRIVER_MIR)
    pd.ndt = wmi.info.mir.connection;
    pd.nwh = wmi.info.mir.surface;
#endif

#if defined(SDL_VIDEO_DRIVER_COCOA)
    pd.ndt = NULL;
    pd.nwh = wmi.info.cocoa.window;
#endif

#if defined(SDL_VIDEO_DRIVER_UIKIT)
    pd.ndt = NULL;
    pd.nwh = wmi.info.uikit.window;
#endif

#if defined(SDL_VIDEO_DRIVER_WINDOWS)
    pd.ndt = NULL;
    pd.nwh = wmi.info.win.window;
#endif

#if defined(SDL_VIDEO_DRIVER_WINRT)
    pd.ndt = NULL;
    pd.nwh = wmi.info.winrt.window;
#endif

#if defined(SDL_VIDEO_DRIVER_VIVANTE)
    pd.ndt = wmi.info.vivante.display;
    pd.nwh = wmi.info.vivante.window;
#endif

    pd.context = NULL;
    pd.backBuffer = NULL;
    pd.backBufferDS = NULL;
}

} //namespace bigsaddle