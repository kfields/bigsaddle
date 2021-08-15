#pragma once

#include <functional>
#include <memory>
#include <string>

//#include <X11/Xlib.h>
typedef unsigned long XWindow;

#include <bigsaddle/shell/window_base.h>

namespace bigsaddle {

class X11Window : public WindowBase {
 public:

    X11Window(WindowParams params = WindowParams()) : WindowBase(params) {}
    virtual ~X11Window();

    virtual void ReDraw() override;

    XWindow GetNativeHandle();
    virtual void* GetHandle() override { return (void*)GetNativeHandle(); }
};

} //namespace bigsaddle