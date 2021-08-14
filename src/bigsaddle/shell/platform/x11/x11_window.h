#pragma once

#include <functional>
#include <memory>
#include <string>

//#include <X11/Xlib.h>
typedef unsigned long XWindow;

#include <bigsaddle/shell/window_base.h>

class X11Window : public WindowBase {
 public:

    X11Window(CreateParams params = CreateParams()) : WindowBase(params) {}
    virtual ~X11Window();

    virtual void ReDraw() override;

    XWindow GetNativeHandle();
    virtual void* GetHandle() override { return (void*)GetNativeHandle(); }
};
