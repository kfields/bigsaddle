#pragma once

#include "shell/window.h"

namespace bigsaddle {

class App : public Window {
public:
    enum class State {
        kRunning,
        kShutdown
    };

    App(WindowParams params = WindowParams()) : Window(params), state_(State::kRunning), resetFlags_(BGFX_RESET_VSYNC) {}
    virtual ~App();
    virtual void Create(WindowParams params = WindowParams()) override;
    virtual void CreateGfx();
    virtual void CreateGui();
    virtual bool Dispatch(const SDL_Event& event) override;
    virtual void PreRender() override;
    virtual void PostRender() override;
    virtual void Reset() override;

    int Run();

    //Data members
    State state_;
    std::thread paint_thread_;
    uint32_t resetFlags_;

};

} //namespace bigsaddle