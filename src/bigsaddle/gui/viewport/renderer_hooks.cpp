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

#include <bigsaddle/shell/window.h>
#include <bigsaddle/gui/gui.h>
#include <bigsaddle/gui/renderer/gui_renderer.h>

#include "gui_viewport.h"

static void Renderer_CreateWindow(ImGuiViewport* viewport) {

}

static void Renderer_DestroyWindow(ImGuiViewport* viewport) {

}

static void Renderer_SetWindowSize(ImGuiViewport* viewport, ImVec2 size) {

}

static void Renderer_RenderWindow(ImGuiViewport* viewport, void*) {
    /*GuiViewport& vp = *(GuiViewport*)viewport->PlatformUserData;
    vp.Draw();*/
}

static void Renderer_SwapBuffers(ImGuiViewport* viewport, void*) {

}

bool GuiViewport::InitRendererHooks()
{
    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    platform_io.Renderer_CreateWindow = Renderer_CreateWindow;
    platform_io.Renderer_DestroyWindow = Renderer_DestroyWindow;
    platform_io.Renderer_SetWindowSize = Renderer_SetWindowSize;
    platform_io.Renderer_RenderWindow = Renderer_RenderWindow;
    platform_io.Renderer_SwapBuffers = Renderer_SwapBuffers;
    return true;
}
