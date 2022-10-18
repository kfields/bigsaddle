#include <SDL.h>
#include <SDL_syswm.h>

#include <imgui/imgui.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include <bigsaddle/shell/window.h>
#include <bigsaddle/gui/gui.h>
#include <bigsaddle/gui/gui_renderer.h>

#include "gui_viewport.h"

namespace bigsaddle
{

  GuiViewport::GuiViewport(WindowParams params) : Window(params),
                                                  viewport_(nullptr)
  {
    // viewId_ |= 0x7F;
  }

  void GuiViewport::Create()
  {
    Window::Create();
    Reset();
  }

  void GuiViewport::Destroy()
  {
    if (bgfx::isValid(frameBuffer_))
    {
      destroy(frameBuffer_);
      frameBuffer_.idx = BGFX_INVALID_HANDLE;
      bgfx::setViewFrameBuffer(viewId(), BGFX_INVALID_HANDLE);
      bgfx::frame();
    }

    Window::Destroy();
  }

  void GuiViewport::PostRender()
  {
    gui().renderer().Render(viewId(), viewport_->DrawData);
    Window::PostRender();
  }

  void GuiViewport::Reset(ResetKind kind)
  {
    if (kind == ResetKind::kSoft)
    {
      if (bgfx::isValid(frameBuffer_))
        bgfx::setViewFrameBuffer(viewId(), frameBuffer_);
      return;
    }

    if (bgfx::isValid(frameBuffer_))
      destroy(frameBuffer_);
    void *nh = GetHandle();
    frameBuffer_ = bgfx::createFrameBuffer(nh, width(), height(), bgfx::TextureFormat::BGRA8);
    bgfx::setViewFrameBuffer(viewId(), frameBuffer_);
  }

} // namespace bigsaddle