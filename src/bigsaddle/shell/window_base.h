#pragma once

#include <string>
#include <list>
#include <map>
#include <thread>

#include "dispatcher.h"

class Gui;

struct Point {
    int x;
    int y;
    Point() : x(0), y(0) {}
    Point(int x, int y) : x(x), y(y) {}
    Point(const Point& p1) { x = p1.x; y = p1.y; }
};

struct Size {
    int width;
    int height;
    Size() : width(0), height(0) {}
    Size(int width, int height)
        : width(width), height(height) {}
    Size(const Size& s1) { width = s1.width; height = s1.height; }
};

class WindowBase : public Dispatcher
{
public:
    //
    // Create
    //
    struct CreateParams {
        CreateParams(std::string _title = "Big Saddle",
            Point _origin = Point(0,0),
            Size _size = Size(800,600),
            uint32_t _flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
        ) {
            title = _title;
            origin = _origin;
            size = _size;
            flags = _flags;
        }
        std::string title;
        Point origin;
        Size size;
        uint32_t flags;
    };

    WindowBase(CreateParams& params = CreateParams());
    virtual ~WindowBase();
    bool Create(CreateParams params = CreateParams()) {
        if(!PreCreate(params)) { return false; }
        if(!DoCreate(params)) { return false; }
        if(!PostCreate(params)) { return false; }
        return true;
    }

    virtual bool PreCreate(CreateParams params){ return true; }
    virtual bool DoCreate(CreateParams params);
    virtual bool PostCreate(CreateParams params){ return true; }

    bool CreateAndShow(CreateParams params = CreateParams()) {
        if(!Create(params)) { return false; }
        return Show();
    }
    virtual bool Show();
    virtual bool Hide(){ return true; }

    static bool Startup();
    static void Shutdown();

    virtual void Destroy();
    virtual void DestroyGui(){}

    void Run();

    virtual bool Dispatch(const SDL_Event& event);
    virtual bool DispatchWindowEvent(const SDL_Event& event) override;

    void Draw() { PreDraw(); DoDraw(); PostDraw(); }
    virtual void PreDraw() {}
    virtual void DoDraw() {}
    virtual void PostDraw() {}
    virtual void ReDraw() {}
    virtual void OnSize();
    virtual void Reset() {}
    //
    virtual void* GetHandle() = 0;
    //
    void SetPosition(Point& origin);
    void SetSize(Size& size);
    void AddChild(WindowBase& child) {
        children_.push_back(&child);
        childMap_[child.windowId_] = &child;
    }
    //Accessors
    int x() { return origin_.x; }
    int y() { return origin_.y; }
    int width() { return size_.width; }
    int height() { return size_.height; }
    Gui& gui() { return *gui_; }
    //Data members
    std::string title_;
    Point origin_;
    Size size_;
    uint32_t flags_;
    SDL_Window* window_;
    Gui* gui_;
    uint16_t viewId_;
    static uint16_t instanceCount_;
    uint32_t windowId_;
    std::list<WindowBase*> children_;
    std::map<uint32_t, WindowBase*> childMap_;
};