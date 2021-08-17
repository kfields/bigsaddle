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

} //namespace bigsaddle