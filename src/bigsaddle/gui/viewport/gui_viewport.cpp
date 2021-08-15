#include <SDL.h>
#include <SDL_syswm.h>

#include <imgui/imgui.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include <bigsaddle/shell/window.h>
#include <bigsaddle/gui/gui.h>
#include <bigsaddle/gui/renderer/gui_renderer.h>

#include "gui_viewport.h"

namespace bigsaddle {

GuiViewport::GuiViewport(WindowParams params) : Window(params),
    viewport_(nullptr)
{
}

void GuiViewport::DoCreate(WindowParams params) {
    Window::DoCreate(params);
    Reset();
}

void GuiViewport::DoDraw() {
    Window::DoDraw();
    gui().renderer().Render(viewId(), viewport_->DrawData);
}

void GuiViewport::Reset()
{
    if (bgfx::isValid(frameBuffer_))
        destroy(frameBuffer_);
    void* nh = GetHandle();
    frameBuffer_ = bgfx::createFrameBuffer(nh, width(), height());
    bgfx::setViewFrameBuffer(viewId(), frameBuffer_);
}

} //namespace bigsaddle