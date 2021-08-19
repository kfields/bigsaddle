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

    if (bgfx::isValid(frameBuffer_))
        destroy(frameBuffer_);

    delete this;
}


} //namespace bigsaddle