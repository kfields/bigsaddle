#pragma once

#include "shell/window.h"

namespace bigsaddle {

class App : public Window {
public:
    enum class State {
        kRunning,
        kShutdown
    };

    App(CreateParams params = CreateParams()) : Window(params), state_(State::kRunning), resetFlags_(BGFX_RESET_VSYNC) {}
    virtual ~App();
    virtual void DoCreate(CreateParams params) override;
    virtual void PostCreate(WindowBase::CreateParams params) override;
    virtual bool CreateGui(CreateParams params);
    virtual bool Dispatch(const SDL_Event& event) override;
    virtual void Destroy() override;
    virtual void PreDraw() override;
    virtual void DoDraw() override;
    virtual void PostDraw() override;
    virtual void Reset() override;

    int Run();

    //Data members
    State state_;
    std::thread paint_thread_;
    uint32_t resetFlags_;

};

} //namespace bigsaddle