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

App::~App() {
    paint_thread_.detach();
}

bool App::DoCreate(CreateParams params) {
    Window::DoCreate(params);
    auto result = bgfx::renderFrame(); // single threaded mode

    bgfx::PlatformData pd{};

    pd.nwh = GetHandle();

    bgfx::Init bgfx_init;
    bgfx_init.type = bgfx::RendererType::Count; // auto choose renderer
    bgfx_init.resolution.width = width();
    bgfx_init.resolution.height = height();
    bgfx_init.resolution.reset = resetFlags_;
    bgfx_init.platformData = pd;
    bgfx::init(bgfx_init);

    bgfx::setViewClear(
        viewId_, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x11111111, 1.0f, 0);
    bgfx::setViewRect(viewId_, 0, 0, width(), height());


    return CreateGui(params);
}

bool App::PostCreate(WindowBase::CreateParams params) {
    paint_thread_ = std::thread([this]() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
            this->ReDraw();
        }
    });

    return Window::PostCreate(params);
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
    gui().Destroy();
    Window::Destroy();
}

void App::PreDraw() {
    gui().NewFrame();
    Window::PreDraw();
}

void App::DoDraw() {
    Window::DoDraw();
    bgfx::touch(viewId_);
}

void App::PostDraw() {
    gui().Render();
    bgfx::frame();
    Window::PostDraw();
}
