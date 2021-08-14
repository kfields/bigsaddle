#pragma once

#include <functional>
#include <memory>
#include <string>

#include <windows.h>

#include <bigsaddle/shell/window_base.h>

namespace bigsaddle {

class Win32Window : public WindowBase {
 public:

    Win32Window(CreateParams& params = CreateParams()) : WindowBase(params) {}
    virtual ~Win32Window();

    virtual void ReDraw() override;

    int AttachTo(HWND hParent);
    HWND GetNativeHandle();
    virtual void* GetHandle() override { return (void*)GetNativeHandle(); }
    RECT GetClientArea();

};

} //namespace bigsaddle {