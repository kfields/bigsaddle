#pragma once

#include <bigsaddle/shell/window.h>

struct SDL_Window;
struct SDL_Cursor;

typedef union SDL_Event SDL_Event;

namespace bigsaddle {

class Gui;
class GuiRenderer;

class GuiViewport : public Window
{
public:
    GuiViewport(WindowParams params = WindowParams());
    ~GuiViewport() {}

    virtual void Create(WindowParams params) override;
    virtual void Destroy() override;
    
    static GuiViewport& Produce(WindowParams params) {
        GuiViewport& r = *new GuiViewport(params);
        r.Create(params);
        return r;
    }
    virtual void PostRender();
    virtual void Reset() override;

    //Data members
    ImGuiViewport* viewport_;
    bgfx::FrameBufferHandle frameBuffer_;
};

} //namespace bigsaddle