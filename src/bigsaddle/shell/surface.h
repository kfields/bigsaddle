#pragma once

#include <string>
#include <list>
#include <map>

#include <bgfx/bgfx.h>

#include "dispatcher.h"

namespace bigsaddle {

typedef std::pair<uint32_t, uint16_t> SurfaceId;

class Surface : public Dispatcher {
public:
    Surface();
    virtual ~Surface();
    void AddChild(Surface& child) {
        child.parent_ = this;
        children_.push_back(&child);
        childMap_[child.id_] = &child;
    }
    void RemoveChild(Surface& child) {
        child.parent_ = nullptr;
        children_.remove(&child);
        childMap_.erase(child.id_);
    }

    void Render() { PreRender(); Draw(); PostRender(); }
    virtual void PreRender() {}
    virtual void Draw() {}
    virtual void PostRender() {}
    virtual void ReRender() {}

    //Accessors
    uint32_t windowId() { return id_.first; }
    void SetWindowId(uint32_t id) { id_.first = id; }
    uint16_t viewId() { return id_.second; }
    void SetViewId(uint16_t id) { id_.second = id; }
    //Data members
    static uint16_t viewCount_;

    SurfaceId id_;
    Surface* parent_;
    std::list<Surface*> children_;
    std::map<SurfaceId, Surface*> childMap_;
    bgfx::FrameBufferHandle frameBuffer_;
};

} //namespace bigsaddle