#pragma once

#include <bigsaddle/shell/window.h>

struct SDL_Window;
struct SDL_Cursor;

typedef union SDL_Event SDL_Event;

class Gui;
class GuiRenderer;

class GuiViewport : public Window
{
public:
    GuiViewport(CreateParams& params = CreateParams());
    ~GuiViewport() {}

    virtual bool DoCreate(CreateParams params) override;
    virtual void DoDraw();
    virtual void Reset() override;

    static bool InitHooks(Gui& gui, void* sdl_gl_context) {
        if(InitPlatformHooks(gui, sdl_gl_context)) {
            return InitRendererHooks();
        }
        return false;
    }
    static bool InitPlatformHooks(Gui& gui, void* sdl_gl_context);
    static bool InitRendererHooks();
    //Data members
    ImGuiViewport* viewport_;
    bgfx::FrameBufferHandle frameBuffer_;
};
