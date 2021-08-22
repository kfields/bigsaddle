#pragma once

#include <imgui/imgui.h>

struct SDL_Window;
struct SDL_Cursor;
typedef union SDL_Event SDL_Event;

namespace bigsaddle {

class App;
class GuiRenderer;

class Gui {
public:
    Gui(App& app);
    virtual ~Gui();
    void Create();
    static Gui& Produce(App& app) {
        Gui& r = *new Gui(app);
        r.Create();
        return r;
    }
    void Init();
    void InitHooks();
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
};

} //namespace bigsaddle