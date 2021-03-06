#include "surface.h"

#include <bgfx/bgfx.h>

namespace bigsaddle {

//TODO:Recycle handles
uint16_t Surface::viewCount_ = 0;

Surface::Surface() : 
    viewId_(viewCount_++), parent_(nullptr), frameBuffer_(BGFX_INVALID_HANDLE)
{
}

Surface::~Surface() {
}

void Surface::Destroy() {
    for (auto child : children_) {
        child->parent_ = nullptr;
        child->Destroy();
    }
    if (parent_ != nullptr) {
        parent_->RemoveChild(*this);
    }

    delete this;
}

uint16_t Surface::AllocViewId() {
    return viewCount_++;
}

void Surface::FreeViewId(uint16_t id) {
    //TODO:Laziness
}


} //namespace bigsaddle