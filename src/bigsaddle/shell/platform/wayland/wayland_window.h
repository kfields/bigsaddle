#pragma once

#include <functional>
#include <memory>
#include <string>

typedef unsigned long XWindow;

#include <bigsaddle/shell/window_base.h>

struct wl_surface;

namespace bigsaddle {

class WaylandWindow : public WindowBase {
 public:

    WaylandWindow(WindowParams params = WindowParams()) : WindowBase(params) {}
    virtual ~WaylandWindow();

    virtual void ReRender() override;

    wl_surface* GetNativeHandle();
    virtual void* GetHandle() override { return (void*)GetNativeHandle(); }
};

} //namespace bigsaddle