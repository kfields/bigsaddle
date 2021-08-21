#include "surface.h"

#include <bgfx/bgfx.h>

namespace bigsaddle {

//TODO:Recycle handles
uint16_t Surface::viewCount_ = 0;

Surface::Surface() :
    viewId_(viewCount_++), parent_(nullptr)
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


} //namespace bigsaddle