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

    virtual void Create();
    virtual void Destroy() override;

    void CreateAndShow() {
        Create();
        Show();
    }
    virtual void Show();
    virtual void Hide() {}

    virtual bool Dispatch(const SDL_Event& event);
    virtual bool DispatchWindowEvent(const SDL_Event& event) override;

    virtual void OnSize();
    virtual void Reset() {}
    //
    virtual void* GetHandle() { return nullptr; }
    //
    void SetPosition(Point origin);
    void SetSize(Size size);
    //
    static void MapWindow(uint32_t key, WindowBase* window) { windowMap_[key] = window; }
    static void UnmapWindow(uint32_t key) { windowMap_.erase(key); }
    //Accessors
    uint32_t windowId() { return windowId_; }
    void SetWindowId(uint32_t id) { windowId_ = id; }
    int x() { return origin_.x; }
    int y() { return origin_.y; }
    int width() { return size_.width; }
    int height() { return size_.height; }
    Gui& gui() { return *gui_; }
    //Data members
    uint32_t windowId_;
    static std::map<uint32_t, WindowBase*> windowMap_;
    std::string title_;
    Point origin_;
    Size size_;
    uint32_t flags_;
    SDL_Window* window_;
    Gui* gui_;
};

} //namespace bigsaddle