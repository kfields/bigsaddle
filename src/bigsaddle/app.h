#pragma once

#include "shell/window.h"

class App : public Window {
public:
    App(CreateParams& params = CreateParams()) : Window(params), resetFlags_(BGFX_RESET_VSYNC) {}
    virtual ~App();
    virtual bool DoCreate(CreateParams params) override;
    virtual bool PostCreate(WindowBase::CreateParams params) override;
    virtual bool CreateGui(CreateParams params);
    virtual bool Dispatch(const SDL_Event& event) override;
    virtual void Destroy() override;
    virtual void PreDraw() override;
    virtual void DoDraw() override;
    virtual void PostDraw() override;
    virtual void Reset() override;

    //Data members
    std::thread paint_thread_;
    uint32_t resetFlags_;

};