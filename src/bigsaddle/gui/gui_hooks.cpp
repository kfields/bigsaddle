// SDL
// (the multi-viewports feature requires SDL features supported from SDL 2.0.4+. SDL 2.0.5+ is highly recommended)
#include <SDL.h>
#include <SDL_syswm.h>
#if defined(__APPLE__)
#include "TargetConditionals.h"
#endif

#define SDL_HAS_CAPTURE_AND_GLOBAL_MOUSE    (SDL_VERSION_ATLEAST(2,0,4) && !defined(__EMSCRIPTEN__) && !defined(__ANDROID__) && !(defined(__APPLE__) && TARGET_OS_IOS))
#define SDL_HAS_MOUSE_FOCUS_CLICKTHROUGH    SDL_VERSION_ATLEAST(2,0,5)
#define SDL_HAS_WINDOW_ALPHA                SDL_VERSION_ATLEAST(2,0,5)
#define SDL_HAS_ALWAYS_ON_TOP               SDL_VERSION_ATLEAST(2,0,5)
#define SDL_HAS_USABLE_DISPLAY_BOUNDS       SDL_VERSION_ATLEAST(2,0,5)
#define SDL_HAS_PER_MONITOR_DPI             SDL_VERSION_ATLEAST(2,0,4)
#define SDL_HAS_VULKAN                      SDL_VERSION_ATLEAST(2,0,6)
#if !SDL_HAS_VULKAN
static const Uint32 SDL_WINDOW_VULKAN = 0x10000000;
#endif

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include <bigsaddle/app.h>
#include <bigsaddle/gui/gui.h>

#include "viewport/gui_viewport.h"

namespace bigsaddle {

static void Platform_CreateWindow(ImGuiViewport* viewport)
{
    ImGuiIO& io = ImGui::GetIO();
    Gui& gui = *(Gui*)io.BackendPlatformUserData;

    ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    App& app = *(App*)main_viewport->PlatformUserData;

    Uint32 sdl_flags = 0;
    sdl_flags |= SDL_GetWindowFlags(app.window_) & SDL_WINDOW_ALLOW_HIGHDPI;
    sdl_flags |= SDL_WINDOW_HIDDEN;
    sdl_flags |= (viewport->Flags & ImGuiViewportFlags_NoDecoration) ? SDL_WINDOW_BORDERLESS : 0;
    sdl_flags |= (viewport->Flags & ImGuiViewportFlags_NoDecoration) ? 0 : SDL_WINDOW_RESIZABLE;
#if !defined(_WIN32)
    // See SDL hack in Platform_ShowWindow().
    sdl_flags |= (viewport->Flags & ImGuiViewportFlags_NoTaskBarIcon) ? SDL_WINDOW_SKIP_TASKBAR : 0;
#endif
#if SDL_HAS_ALWAYS_ON_TOP
    sdl_flags |= (viewport->Flags & ImGuiViewportFlags_TopMost) ? SDL_WINDOW_ALWAYS_ON_TOP : 0;
#endif
    WindowParams params = WindowParams("No Title Yet",
        Point((int)viewport->Pos.x, (int)viewport->Pos.y),
        Size((int)viewport->Size.x, (int)viewport->Size.y),
        sdl_flags);
    GuiViewport& vp = GuiViewport::Produce(params);
    app.AddChild(vp);
    vp.gui_ = &gui;
    vp.viewport_ = viewport;

    viewport->PlatformUserData = &vp;
    viewport->PlatformHandle = (void*)vp.window_;
    viewport->PlatformHandleRaw = vp.GetHandle();
}

static void Platform_DestroyWindow(ImGuiViewport* viewport)
{
    Window& wnd = *(Window*)viewport->PlatformUserData;
    if (typeid(wnd) != typeid(App)) {
        delete &wnd;
    }
    viewport->PlatformUserData = viewport->PlatformHandle = NULL;
}

static void Platform_ShowWindow(ImGuiViewport* viewport)
{
    Window& wnd = *(Window*)viewport->PlatformUserData;
    wnd.Show();
}

static ImVec2 Platform_GetWindowPos(ImGuiViewport* viewport)
{
    Window& wnd = *(Window*)viewport->PlatformUserData;
    int x = 0, y = 0;
    SDL_GetWindowPosition(wnd.window_, &x, &y);
    return ImVec2((float)x, (float)y);
}

static void Platform_SetWindowPos(ImGuiViewport* viewport, ImVec2 pos)
{
    Window& wnd = *(Window*)viewport->PlatformUserData;
    wnd.SetPosition(Point((int)pos.x, (int)pos.y));
}

static ImVec2 Platform_GetWindowSize(ImGuiViewport* viewport)
{
    Window& wnd = *(Window*)viewport->PlatformUserData;
    int w = 0, h = 0;
    SDL_GetWindowSize(wnd.window_, &w, &h);
    return ImVec2((float)w, (float)h);
}

static void Platform_SetWindowSize(ImGuiViewport* viewport, ImVec2 size)
{
    Window& wnd = *(Window*)viewport->PlatformUserData;
    wnd.SetSize(Size((int)size.x, (int)size.y));
}

static void Platform_SetWindowTitle(ImGuiViewport* viewport, const char* title)
{
    Window& wnd = *(Window*)viewport->PlatformUserData;
    SDL_SetWindowTitle(wnd.window_, title);
}

#if SDL_HAS_WINDOW_ALPHA
static void Platform_SetWindowAlpha(ImGuiViewport* viewport, float alpha)
{
    Window& wnd = *(Window*)viewport->PlatformUserData;
    SDL_SetWindowOpacity(wnd.window_, alpha);
}
#endif

static void Platform_SetWindowFocus(ImGuiViewport* viewport)
{
    Window& wnd = *(Window*)viewport->PlatformUserData;
    SDL_RaiseWindow(wnd.window_);
}

static bool Platform_GetWindowFocus(ImGuiViewport* viewport)
{
    Window& wnd = *(Window*)viewport->PlatformUserData;
    return (SDL_GetWindowFlags(wnd.window_) & SDL_WINDOW_INPUT_FOCUS) != 0;
}

static bool Platform_GetWindowMinimized(ImGuiViewport* viewport)
{
    Window& wnd = *(Window*)viewport->PlatformUserData;
    return (SDL_GetWindowFlags(wnd.window_) & SDL_WINDOW_MINIMIZED) != 0;
}

static void Platform_RenderWindow(ImGuiViewport* viewport, void*)
{
    Window& wnd = *(Window*)viewport->PlatformUserData;
    wnd.Render();
}

void Gui::InitHooks()
{
    // Register platform interface (will be coupled with a renderer interface)
    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    platform_io.Platform_CreateWindow = Platform_CreateWindow;
    platform_io.Platform_DestroyWindow = Platform_DestroyWindow;
    platform_io.Platform_ShowWindow = Platform_ShowWindow;
    platform_io.Platform_SetWindowPos = Platform_SetWindowPos;
    platform_io.Platform_GetWindowPos = Platform_GetWindowPos;
    platform_io.Platform_SetWindowSize = Platform_SetWindowSize;
    platform_io.Platform_GetWindowSize = Platform_GetWindowSize;
    platform_io.Platform_SetWindowFocus = Platform_SetWindowFocus;
    platform_io.Platform_GetWindowFocus = Platform_GetWindowFocus;
    platform_io.Platform_GetWindowMinimized = Platform_GetWindowMinimized;
    platform_io.Platform_SetWindowTitle = Platform_SetWindowTitle;
    platform_io.Platform_RenderWindow = Platform_RenderWindow;
#if SDL_HAS_WINDOW_ALPHA
    platform_io.Platform_SetWindowAlpha = Platform_SetWindowAlpha;
#endif

    ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    Window& wnd = *app_;
    main_viewport->PlatformUserData = &wnd;
    main_viewport->PlatformHandle = wnd.window_;
}

} //namespace bigsaddle