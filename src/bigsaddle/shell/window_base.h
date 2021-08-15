#pragma once

#include <string>
#include <list>
#include <map>
#include <thread>

#include "surface.h"

namespace bigsaddle {

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

struct WindowParams {
    WindowParams(std::string _title = "Big Saddle",
        Point _origin = Point(SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED),
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

class WindowBase : public Surface
{
public:
    WindowBase(WindowParams params = WindowParams());
    virtual ~WindowBase();

    void Create(WindowParams params = WindowParams()) {
        PreCreate(params);
        DoCreate(params);
        PostCreate(params);
    }

    virtual void PreCreate(WindowParams params){}
    virtual void DoCreate(WindowParams params);
    virtual void PostCreate(WindowParams params){}

    bool CreateAndShow(WindowParams params = WindowParams()) {
        Create(params);
        return Show();
    }
    virtual bool Show();
    virtual bool Hide(){ return true; }

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
    void SetPosition(Point origin);
    void SetSize(Size size);
    //
    static void MapWindow(uint32_t key, WindowBase* window) { windowMap_[key] = window; }
    static void UnmapWindow(uint32_t key) { windowMap_.erase(key); }
    //Accessors
    int x() { return origin_.x; }
    int y() { return origin_.y; }
    int width() { return size_.width; }
    int height() { return size_.height; }
    Gui& gui() { return *gui_; }
    //Data members
    static std::map<uint32_t, WindowBase*> windowMap_;
    std::string title_;
    Point origin_;
    Size size_;
    uint32_t flags_;
    SDL_Window* window_;
    Gui* gui_;
};

} //namespace bigsaddle