#include "surface.h"

#include <bgfx/bgfx.h>

namespace bigsaddle {

//TODO:Recycle handles
uint16_t Surface::viewCount_ = 0;

Surface::Surface() :
    id_(SurfaceId(0, viewCount_++)), frameBuffer_(BGFX_INVALID_HANDLE)
{
}

Surface::~Surface() {
    /*if (parent_ != nullptr) {
        parent_->RemoveChild(*this);
    }*/
}

} //namespace bigsaddle