#pragma once

#include <list>

namespace bigsaddle {

class GuiRenderer {
public:
    GuiRenderer();
    virtual ~GuiRenderer();

    void Create();

    static GuiRenderer& Produce() {
        GuiRenderer& r = *new GuiRenderer();
        r.Create();
        return r;
    }
    void Render(uint16_t viewId, struct ImDrawData* draw_data);

    //Data members
    bgfx::ProgramHandle program_ = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle fontTexture_ = BGFX_INVALID_HANDLE;
    bgfx::VertexLayout vertexLayout_;
    bgfx::UniformHandle attribLocationTex_ = BGFX_INVALID_HANDLE;

	bgfx::ProgramHandle imageProgram_;
	bgfx::UniformHandle imageLodEnabled_;
    bgfx::UniformHandle samplerTex_;
};

} //namespace bigsaddle