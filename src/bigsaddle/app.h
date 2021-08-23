#pragma once

#include "shell/window.h"

namespace bigsaddle {

class App : public Window {
public:
    enum class State {
        kRunning,
        kShutdown
    };

    App(WindowParams params = WindowParams());
    virtual ~App();
    virtual void Create() override;
    virtual void CreateGfx();
    virtual void CreateGui();
    virtual void Destroy() override;
    virtual bool Dispatch(const SDL_Event& event) override;
    virtual void PreRender() override;
    virtual void PostRender() override;
    virtual void Reset(ResetKind kind = ResetKind::kHard) override;

    int Run();

    //Data members
    State state_;
    uint32_t resetFlags_;

};

} //namespace bigsaddle