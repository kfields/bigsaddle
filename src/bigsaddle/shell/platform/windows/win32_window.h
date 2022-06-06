#pragma once

#include <functional>
#include <memory>
#include <string>

#include <windows.h>

#include <bigsaddle/shell/window_base.h>

namespace bigsaddle {

class Win32Window : public WindowBase {
 public:

    Win32Window(WindowParams params = WindowParams()) : WindowBase(params) {}
    virtual ~Win32Window();

    int AttachTo(HWND hParent);
    HWND GetNativeHandle();
    virtual void* GetHandle() override { return (void*)GetNativeHandle(); }
    RECT GetClientArea();

};

} //namespace bigsaddle {