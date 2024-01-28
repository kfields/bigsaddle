// SDL
// (the multi-viewports feature requires SDL features supported from SDL 2.0.4+. SDL 2.0.5+ is highly recommended)
#include <SDL.h>
#if defined(__APPLE__)
#include "TargetConditionals.h"
#endif

#define SDL_HAS_CAPTURE_AND_GLOBAL_MOUSE (SDL_VERSION_ATLEAST(2, 0, 4) && !defined(__EMSCRIPTEN__) && !defined(__ANDROID__) && !(defined(__APPLE__) && TARGET_OS_IOS))
#define SDL_HAS_MOUSE_FOCUS_CLICKTHROUGH SDL_VERSION_ATLEAST(2, 0, 5)
#define SDL_HAS_WINDOW_ALPHA SDL_VERSION_ATLEAST(2, 0, 5)
#define SDL_HAS_ALWAYS_ON_TOP SDL_VERSION_ATLEAST(2, 0, 5)
#define SDL_HAS_USABLE_DISPLAY_BOUNDS SDL_VERSION_ATLEAST(2, 0, 5)
#define SDL_HAS_PER_MONITOR_DPI SDL_VERSION_ATLEAST(2, 0, 4)
#define SDL_HAS_VULKAN SDL_VERSION_ATLEAST(2, 0, 6)
#if !SDL_HAS_VULKAN
static const Uint32 SDL_WINDOW_VULKAN = 0x10000000;
#endif

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include <bigsaddle/app.h>

#include "gui.h"
#include "gui_renderer.h"
#include "gui_viewport.h"

namespace bigsaddle
{

    Gui::Gui(App &app) : app_(&app), window_(app.window_), renderer_(nullptr)
    {
    }

    Gui::~Gui()
    {
        delete renderer_;

        ImGuiIO &io = ImGui::GetIO();
        io.BackendPlatformName = NULL;
        io.BackendPlatformUserData = NULL;
        ImGui::DestroyContext();

        if (clipboardTextData_)
            SDL_free(clipboardTextData_);
        for (ImGuiMouseCursor cursor_n = 0; cursor_n < ImGuiMouseCursor_COUNT; cursor_n++)
            SDL_DestroyCursor(mouseCursors_[cursor_n]);
    }

    void Gui::Create()
    {
        context_ = ImGui::CreateContext();
        io_ = &ImGui::GetIO();
        io().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable | ImGuiConfigFlags_DockingEnable;
        io().BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
        // io().ConfigViewportsNoDecoration = false;
        // io().ConfigViewportsNoTaskBarIcon = true;

        renderer_ = &GuiRenderer::Produce();

        Init();
    }

    void Gui::Render()
    {
        ImGui::Render();
        // uint16_t viewId = app().viewId();
        // TODO: Dynamically allocate viewId?
        uint16_t viewId = 255;
        renderer().Render(viewId, ImGui::GetDrawData());
        // Update and Render additional Platform Windows
        if (!(io().ConfigFlags & ImGuiConfigFlags_ViewportsEnable))
            return;

        ImGui::UpdatePlatformWindows();

        // Skip the main viewport (index 0), which is always fully handled by the application!
        ImGuiPlatformIO &platform_io = ImGui::GetPlatformIO();
        for (int i = 1; i < platform_io.Viewports.Size; i++)
        {
            ImGuiViewport *viewport = platform_io.Viewports[i];
            if (viewport->Flags & ImGuiViewportFlags_IsMinimized)
                continue;
            if (platform_io.Platform_RenderWindow)
                platform_io.Platform_RenderWindow(viewport, nullptr);
        }
    }

    char *Gui::clipboardTextData_;
    // Functions
    const char *Gui::GetClipboardText(void *)
    {
        if (clipboardTextData_)
            SDL_free(clipboardTextData_);
        clipboardTextData_ = SDL_GetClipboardText();
        return clipboardTextData_;
    }

    void Gui::SetClipboardText(void *, const char *text)
    {
        SDL_SetClipboardText(text);
    }

    bool Gui::Dispatch(const SDL_Event *event)
    {
        ImGuiIO &io = ImGui::GetIO();

        switch (event->type)
        {
        case SDL_EVENT_MOUSE_WHEEL:
        {
            if (event->wheel.x > 0)
                io.MouseWheelH += 1;
            if (event->wheel.x < 0)
                io.MouseWheelH -= 1;
            if (event->wheel.y > 0)
                io.MouseWheel += 1;
            if (event->wheel.y < 0)
                io.MouseWheel -= 1;
            return true;
        }
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        {
            if (event->button.button == SDL_BUTTON_LEFT)
            {
                mousePressed_[0] = true;
            }
            if (event->button.button == SDL_BUTTON_RIGHT)
            {
                mousePressed_[1] = true;
            }
            if (event->button.button == SDL_BUTTON_MIDDLE)
            {
                mousePressed_[2] = true;
            }
            return true;
        }
        case SDL_EVENT_TEXT_INPUT:
        {
            io.AddInputCharactersUTF8(event->text.text);
            return true;
        }
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
        {
            int key = event->key.keysym.scancode;
            io.KeysDown[key] = (event->type == SDL_EVENT_KEY_DOWN);
            io.KeyShift = ((SDL_GetModState() & SDL_KMOD_SHIFT) != 0);
            io.KeyCtrl = ((SDL_GetModState() & SDL_KMOD_CTRL) != 0);
            io.KeyAlt = ((SDL_GetModState() & SDL_KMOD_ALT) != 0);
#ifdef _WIN32
            io.KeySuper = false;
#else
            io.KeySuper = ((SDL_GetModState() & SDL_KMOD_GUI) != 0);
#endif
            return true;
        }
        // Multi-viewport support
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        case SDL_EVENT_WINDOW_MOVED:
        case SDL_EVENT_WINDOW_RESIZED:
            if (ImGuiViewport *viewport = ImGui::FindViewportByPlatformHandle((void *)SDL_GetWindowFromID(event->window.windowID)))
            {
                switch (event->type)
                {
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                    viewport->PlatformRequestClose = true;
                    return true;
                case SDL_EVENT_WINDOW_MOVED:
                    viewport->PlatformRequestMove = true;
                    return true;
                case SDL_EVENT_WINDOW_RESIZED:
                    viewport->PlatformRequestResize = true;
                    return true;
                }
            }

            break;
        }
        return false;
    }

    void Gui::Init()
    {
        ImGuiIO &io = ImGui::GetIO();
        IM_ASSERT(io.BackendPlatformUserData == NULL && "Already initialized a platform backend!");

        // Check and store if we are on a SDL backend that supports global mouse position
        // ("wayland" and "rpi" don't support it, but we chose to use a white-list instead of a black-list)
        const char *sdl_backend = SDL_GetCurrentVideoDriver();
        const char *global_mouse_whitelist[] = {"windows", "cocoa", "x11", "DIVE", "VMAN"};

#if SDL_HAS_CAPTURE_AND_GLOBAL_MOUSE
        for (int n = 0; n < IM_ARRAYSIZE(global_mouse_whitelist); n++)
            if (strncmp(sdl_backend, global_mouse_whitelist[n], strlen(global_mouse_whitelist[n])) == 0)
                mouseCanUseGlobalState_ = true;
#endif

        // Setup backend capabilities flags
        io.BackendPlatformUserData = (void *)this;
        io.BackendPlatformName = "BigSaddle";
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; // We can honor GetMouseCursor() values (optional)
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;  // We can honor io.WantSetMousePos requests (optional, rarely used)
        if (mouseCanUseGlobalState_)
            io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports; // | ImGuiBackendFlags_RendererHasViewports;  // We can create multi-viewports on the Platform side (optional)

        // Keyboard mapping. Dear ImGui will use those indices to peek into the io.KeysDown[] array.
        io.KeyMap[ImGuiKey_Tab] = SDL_SCANCODE_TAB;
        io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
        io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
        io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
        io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
        io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
        io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
        io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
        io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
        io.KeyMap[ImGuiKey_Insert] = SDL_SCANCODE_INSERT;
        io.KeyMap[ImGuiKey_Delete] = SDL_SCANCODE_DELETE;
        io.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;
        io.KeyMap[ImGuiKey_Space] = SDL_SCANCODE_SPACE;
        io.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN;
        io.KeyMap[ImGuiKey_Escape] = SDL_SCANCODE_ESCAPE;
        io.KeyMap[ImGuiKey_KeypadEnter] = SDL_SCANCODE_KP_ENTER;
        io.KeyMap[ImGuiKey_A] = SDL_SCANCODE_A;
        io.KeyMap[ImGuiKey_C] = SDL_SCANCODE_C;
        io.KeyMap[ImGuiKey_V] = SDL_SCANCODE_V;
        io.KeyMap[ImGuiKey_X] = SDL_SCANCODE_X;
        io.KeyMap[ImGuiKey_Y] = SDL_SCANCODE_Y;
        io.KeyMap[ImGuiKey_Z] = SDL_SCANCODE_Z;

        io.SetClipboardTextFn = SetClipboardText;
        io.GetClipboardTextFn = GetClipboardText;
        io.ClipboardUserData = NULL;

        // Load mouse cursors
        mouseCursors_[ImGuiMouseCursor_Arrow] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
        mouseCursors_[ImGuiMouseCursor_TextInput] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
        mouseCursors_[ImGuiMouseCursor_ResizeAll] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
        mouseCursors_[ImGuiMouseCursor_ResizeNS] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
        mouseCursors_[ImGuiMouseCursor_ResizeEW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
        mouseCursors_[ImGuiMouseCursor_ResizeNESW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
        mouseCursors_[ImGuiMouseCursor_ResizeNWSE] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
        mouseCursors_[ImGuiMouseCursor_Hand] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
        mouseCursors_[ImGuiMouseCursor_NotAllowed] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO);

        // Our mouse update function expect PlatformHandle to be filled for the main viewport
        ImGuiViewport *main_viewport = ImGui::GetMainViewport();
        main_viewport->PlatformHandle = (void *)window_;
#ifdef _WIN32
        SDL_SysWMinfo info;
        if (0 == SDL_GetWindowWMInfo(window_, &info, SDL_SYSWM_CURRENT_VERSION))
        {
            main_viewport->PlatformHandleRaw = info.info.win.window;
        }
        else
        {
            printf(
                "SDL_SysWMinfo could not be retrieved. SDL_Error: %s\n",
                SDL_GetError());
        }
#endif

        // Set SDL hint to receive mouse click events on window focus, otherwise SDL doesn't emit the event.
        // Without this, when clicking to gain focus, our widgets wouldn't activate even though they showed as hovered.
        // (This is unfortunately a global SDL setting, so enabling it might have a side-effect on your application.
        // It is unlikely to make a difference, but if your app absolutely needs to ignore the initial on-focus click:
        // you can ignore SDL_MOUSEBUTTONDOWN events coming right after a SDL_EVENT_WINDOW_FOCUS_GAINED)
#if SDL_HAS_MOUSE_FOCUS_CLICKTHROUGH
        SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
#endif

        // Update monitors
        UpdateMonitors();

        // We need SDL_CaptureMouse(), SDL_GetGlobalMouseState() from SDL 2.0.4+ to support multiple viewports.
        // We left the call to ImGui_ImplSDL2_InitPlatformInterface() outside of #ifdef to avoid unused-function warnings.
        if ((io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) && (io.BackendFlags & ImGuiBackendFlags_PlatformHasViewports))
            InitHooks();
    }

    // This code is incredibly messy because some of the functions we need for full viewport support are not available in SDL < 2.0.4.
    void Gui::UpdateMousePosAndButtons()
    {
        ImGuiIO &io = ImGui::GetIO();

        ImVec2 mouse_pos_prev = io.MousePos;
        io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
        io.MouseHoveredViewport = 0;

        // Update mouse buttons
        float mouse_x_local, mouse_y_local;
        Uint32 mouse_buttons = SDL_GetMouseState(&mouse_x_local, &mouse_y_local);
        io.MouseDown[0] = mousePressed_[0] || (mouse_buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0; // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
        io.MouseDown[1] = mousePressed_[1] || (mouse_buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
        io.MouseDown[2] = mousePressed_[2] || (mouse_buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
        mousePressed_[0] = mousePressed_[1] = mousePressed_[2] = false;

        // Obtain focused and hovered window. We forward mouse input when focused or when hovered (and no other window is capturing)
#if SDL_HAS_CAPTURE_AND_GLOBAL_MOUSE
        SDL_Window *focused_window = SDL_GetKeyboardFocus();
        SDL_Window *hovered_window = SDL_HAS_MOUSE_FOCUS_CLICKTHROUGH ? SDL_GetMouseFocus() : NULL; // This is better but is only reliably useful with SDL 2.0.5+ and SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH.
        SDL_Window *mouse_window = NULL;
        if (hovered_window && (window_ == hovered_window || ImGui::FindViewportByPlatformHandle((void *)hovered_window)))
            mouse_window = hovered_window;
        else if (focused_window && (window_ == focused_window || ImGui::FindViewportByPlatformHandle((void *)focused_window)))
            mouse_window = focused_window;

        // SDL_CaptureMouse() let the OS know e.g. that our imgui drag outside the SDL window boundaries shouldn't e.g. trigger other operations outside
        //SDL_CaptureMouse(ImGui::IsAnyMouseDown() ? SDL_TRUE : SDL_FALSE);
#else
        // SDL 2.0.3 and non-windowed systems: single-viewport only
        SDL_Window *mouse_window = (SDL_GetWindowFlags(bd->Window) & SDL_WINDOW_INPUT_FOCUS) ? bd->Window : NULL;
#endif

        if (mouse_window == NULL)
            return;

        // Set OS mouse position from Dear ImGui if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
        if (io.WantSetMousePos)
        {
#if SDL_HAS_CAPTURE_AND_GLOBAL_MOUSE
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
                SDL_WarpMouseGlobal((int)mouse_pos_prev.x, (int)mouse_pos_prev.y);
            else
#endif
                SDL_WarpMouseInWindow(window_, (int)mouse_pos_prev.x, (int)mouse_pos_prev.y);
        }

        // Set Dear ImGui mouse position from OS position + get buttons. (this is the common behavior)
        if (mouseCanUseGlobalState_)
        {
            float mouse_x_global, mouse_y_global;
            SDL_GetGlobalMouseState(&mouse_x_global, &mouse_y_global);
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                // Multi-viewport mode: mouse position in OS absolute coordinates (io.MousePos is (0,0) when the mouse is on the upper-left of the primary monitor)
                io.MousePos = ImVec2(mouse_x_global, mouse_y_global);
            }
            else
            {
                // Single-viewport mode: mouse position in client window coordinates (io.MousePos is (0,0) when the mouse is on the upper-left corner of the app window)
                // Unlike local position obtained earlier this will be valid when straying out of bounds.
                int window_x, window_y;
                SDL_GetWindowPosition(mouse_window, &window_x, &window_y);
                io.MousePos = ImVec2((float)(mouse_x_global - window_x), (float)(mouse_y_global - window_y));
            }
        }
        else
        {
            io.MousePos = ImVec2((float)mouse_x_local, (float)mouse_y_local);
        }
    }

    void Gui::UpdateMouseCursor()
    {
        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
            return;

        ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
        if (io.MouseDrawCursor || imgui_cursor == ImGuiMouseCursor_None)
        {
            // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
            SDL_HideCursor();
        }
        else
        {
            // Show OS mouse cursor
            SDL_SetCursor(mouseCursors_[imgui_cursor] ? mouseCursors_[imgui_cursor] : mouseCursors_[ImGuiMouseCursor_Arrow]);
            SDL_ShowCursor();
        }
    }

    static void ImGui_ImplSDL2_UpdateGamepads()
    {
        ImGuiIO &io = ImGui::GetIO();
        memset(io.NavInputs, 0, sizeof(io.NavInputs));
        if ((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0)
            return;

        // Get gamepad
        SDL_Gamepad *game_controller = SDL_OpenGamepad(0);
        if (!game_controller)
        {
            io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
            return;
        }

// Update gamepad inputs
#define MAP_BUTTON(NAV_NO, BUTTON_NO)                                                                 \
    {                                                                                                 \
        io.NavInputs[NAV_NO] = (SDL_GetGamepadButton(game_controller, BUTTON_NO) != 0) ? 1.0f : 0.0f; \
    }
#define MAP_ANALOG(NAV_NO, AXIS_NO, V0, V1)                                                       \
    {                                                                                             \
        float vn = (float)(SDL_GetGamepadAxis(game_controller, AXIS_NO) - V0) / (float)(V1 - V0); \
        if (vn > 1.0f)                                                                            \
            vn = 1.0f;                                                                            \
        if (vn > 0.0f && io.NavInputs[NAV_NO] < vn)                                               \
            io.NavInputs[NAV_NO] = vn;                                                            \
    }
        const int thumb_dead_zone = 8000;                                       // SDL_gamecontroller.h suggests using this value.
        MAP_BUTTON(ImGuiNavInput_Activate, SDL_GAMEPAD_BUTTON_SOUTH);               // Cross / A
        MAP_BUTTON(ImGuiNavInput_Cancel, SDL_GAMEPAD_BUTTON_EAST);                 // Circle / B
        MAP_BUTTON(ImGuiNavInput_Menu, SDL_GAMEPAD_BUTTON_WEST);                   // Square / X
        MAP_BUTTON(ImGuiNavInput_Input, SDL_GAMEPAD_BUTTON_NORTH);                  // Triangle / Y
        MAP_BUTTON(ImGuiNavInput_DpadLeft, SDL_GAMEPAD_BUTTON_DPAD_LEFT);       // D-Pad Left
        MAP_BUTTON(ImGuiNavInput_DpadRight, SDL_GAMEPAD_BUTTON_DPAD_RIGHT);     // D-Pad Right
        MAP_BUTTON(ImGuiNavInput_DpadUp, SDL_GAMEPAD_BUTTON_DPAD_UP);           // D-Pad Up
        MAP_BUTTON(ImGuiNavInput_DpadDown, SDL_GAMEPAD_BUTTON_DPAD_DOWN);       // D-Pad Down
        MAP_BUTTON(ImGuiNavInput_FocusPrev, SDL_GAMEPAD_BUTTON_LEFT_SHOULDER);  // L1 / LB
        MAP_BUTTON(ImGuiNavInput_FocusNext, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER); // R1 / RB
        MAP_BUTTON(ImGuiNavInput_TweakSlow, SDL_GAMEPAD_BUTTON_LEFT_SHOULDER);  // L1 / LB
        MAP_BUTTON(ImGuiNavInput_TweakFast, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER); // R1 / RB
        MAP_ANALOG(ImGuiNavInput_LStickLeft, SDL_GAMEPAD_AXIS_LEFTX, -thumb_dead_zone, -32768);
        MAP_ANALOG(ImGuiNavInput_LStickRight, SDL_GAMEPAD_AXIS_LEFTX, +thumb_dead_zone, +32767);
        MAP_ANALOG(ImGuiNavInput_LStickUp, SDL_GAMEPAD_AXIS_LEFTY, -thumb_dead_zone, -32767);
        MAP_ANALOG(ImGuiNavInput_LStickDown, SDL_GAMEPAD_AXIS_LEFTY, +thumb_dead_zone, +32767);

        io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
#undef MAP_BUTTON
#undef MAP_ANALOG
    }

    // FIXME-PLATFORM: SDL doesn't have an event to notify the application of display/monitor changes
    void Gui::UpdateMonitors()
    {
        ImGuiPlatformIO &platform_io = ImGui::GetPlatformIO();
        platform_io.Monitors.resize(0);
        // int display_count = SDL_GetNumVideoDisplays();
        int display_count;
        SDL_DisplayID *displays = SDL_GetDisplays(&display_count);
        for (int n = 0; n < display_count; n++)
        {
            SDL_DisplayID id = displays[n];
            // Warning: the validity of monitor DPI information on Windows depends on the application DPI awareness settings, which generally needs to be set in the manifest or at runtime.
            ImGuiPlatformMonitor monitor;
            SDL_Rect r;
            SDL_GetDisplayBounds(id, &r);
            monitor.MainPos = monitor.WorkPos = ImVec2((float)r.x, (float)r.y);
            monitor.MainSize = monitor.WorkSize = ImVec2((float)r.w, (float)r.h);
#if SDL_HAS_USABLE_DISPLAY_BOUNDS
            SDL_GetDisplayUsableBounds(id, &r);
            monitor.WorkPos = ImVec2((float)r.x, (float)r.y);
            monitor.WorkSize = ImVec2((float)r.w, (float)r.h);
#endif
#if SDL_HAS_PER_MONITOR_DPI
            /*float dpi = 0.0f;
            if (!SDL_GetDisplayPhysicalDPI(id, &dpi, NULL, NULL))
                monitor.DpiScale = dpi / 96.0f;*/
            //const SDL_DisplayMode *SDL_GetCurrentDisplayMode(SDL_DisplayID displayID)
            const SDL_DisplayMode* displayMode = SDL_GetCurrentDisplayMode(id);
            monitor.DpiScale = displayMode->pixel_density;
#endif
            platform_io.Monitors.push_back(monitor);
        }
    }

    void Gui::NewFrame()
    {
        // Setup display size (every frame to accommodate for window resizing)
        int w, h;
        int display_w, display_h;
        SDL_GetWindowSize(window_, &w, &h);
        if (SDL_GetWindowFlags(window_) & SDL_WINDOW_MINIMIZED)
            w = h = 0;
        SDL_GetWindowSizeInPixels(window_, &display_w, &display_h);
        io().DisplaySize = ImVec2((float)w, (float)h);
        if (w > 0 && h > 0)
            io().DisplayFramebufferScale = ImVec2((float)display_w / w, (float)display_h / h);

        // Setup time step (we don't use SDL_GetTicks() because it is using millisecond resolution)
        static uint64_t frequency = SDL_GetPerformanceFrequency();
        uint64_t current_time = SDL_GetPerformanceCounter();
        io().DeltaTime = time_ > 0 ? (float)((double)(current_time - time_) / frequency) : (float)(1.0f / 60.0f);
        time_ = current_time;

        UpdateMousePosAndButtons();
        UpdateMouseCursor();

        // Update game controllers (if enabled and available)
        ImGui_ImplSDL2_UpdateGamepads();

        ImGui::NewFrame();
    }

} // namespace bigsaddle