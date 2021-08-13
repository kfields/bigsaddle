#pragma once

#include <imgui/imgui.h>

struct SDL_Window;
struct SDL_Cursor;
typedef union SDL_Event SDL_Event;

class App;
class GuiRenderer;

class Gui {
public:
    Gui(App& app);

    bool Create();
    bool Init(void* sdl_gl_context = nullptr);
    void Destroy();
    void NewFrame();
    void Render();
    bool Dispatch(const SDL_Event* event);
    //
    static const char* GetClipboardText(void*);
    static void SetClipboardText(void*, const char* text);
    void UpdateMousePosAndButtons();
    void UpdateMouseCursor();
    void UpdateMonitors();
    //Accessors
    App& app() { return *app_; }
    GuiRenderer& renderer() { return *renderer_; }
    ImGuiContext& context() { return *context_; }
    ImGuiIO& io() { return *io_; }
    //Data members
    App* app_ = nullptr;
    SDL_Window* window_ = nullptr;
    GuiRenderer* renderer_ = nullptr;
    //
    ImGuiContext* context_ = nullptr;
    ImGuiIO* io_ = nullptr;
    //
    Uint64      time_ = 0;
    bool        mousePressed_[3];
    SDL_Cursor* mouseCursors_[ImGuiMouseCursor_COUNT];
    static char* clipboardTextData_;
    bool        mouseCanUseGlobalState_ = false;
    bool        useVulkan_ = false;
};
