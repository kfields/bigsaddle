#pragma once

#include <string>
#include <list>
#include <map>

#include <bgfx/bgfx.h>

#include "dispatcher.h"

namespace bigsaddle {

class Surface : public Dispatcher {
public:
    Surface();
    virtual ~Surface();

    virtual void Destroy();

    void AddChild(Surface& child) {
        child.parent_ = this;
        children_.push_back(&child);
    }

    void RemoveChild(Surface& child) {
        child.parent_ = nullptr;
        children_.remove(&child);
    }

    void Render() { PreRender(); Draw(); PostRender(); }
    virtual void PreRender() {}
    virtual void Draw() {}
    virtual void PostRender() {}

    //Accessors
    uint16_t viewId() { return viewId_; }
    void SetViewId(uint16_t id) { viewId_ = id; }
    //Data members
    static uint16_t viewCount_;

    uint16_t viewId_;
    Surface* parent_;
    std::list<Surface*> children_;
};

} //namespace bigsaddle