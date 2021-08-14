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
    GuiViewport(CreateParams params = CreateParams());
    ~GuiViewport() {}

    virtual void DoCreate(CreateParams params) override;
    static GuiViewport& Produce(CreateParams params) {
        GuiViewport& r = *new GuiViewport(params);
        r.Create(params);
        return r;
    }
    virtual void DoDraw();
    virtual void Reset() override;

    static bool InitHooks(Gui& gui);
    //Data members
    ImGuiViewport* viewport_;
};

} //namespace bigsaddle