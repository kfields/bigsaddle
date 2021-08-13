#include <SDL.h>
#include <SDL_syswm.h>

#include <imgui/imgui.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include <bigsaddle/shell/window.h>
#include <bigsaddle/gui/gui.h>
#include <bigsaddle/gui/renderer/gui_renderer.h>

#include "gui_viewport.h"


GuiViewport::GuiViewport(CreateParams& params) : Window(params),
    frameBuffer_(BGFX_INVALID_HANDLE), viewport_(nullptr)
{
}

bool GuiViewport::DoCreate(CreateParams params) {
    Window::DoCreate(params);
    Reset();
    return true;
}

void GuiViewport::DoDraw() {
    Window::DoDraw();
    gui().renderer().Render(viewId_, viewport_->DrawData);
}

void GuiViewport::Reset()
{
    if (bgfx::isValid(frameBuffer_))
        destroy(frameBuffer_);
    void* nh = GetHandle();
    frameBuffer_ = bgfx::createFrameBuffer(nh, width(), height());
    bgfx::setViewFrameBuffer(viewId_, frameBuffer_);
}
