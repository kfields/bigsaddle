#pragma once

#include <list>

namespace ImGui
{
    struct Font
    {
        enum Enum
        {
            Regular,
            Mono,
            Count
        };
    };
};

class GuiRenderer {
public:
    GuiRenderer();
    bool Create();
    static GuiRenderer& Produce() {
        GuiRenderer& r = *new GuiRenderer();
        r.Create();
        return r;
    }
    void Shutdown();
    void Render(uint16_t viewId, struct ImDrawData* draw_data);

    // Use if you want to reset your rendering device without losing ImGui state.
    void InvalidateDeviceObjects();
    bool CreateDeviceObjects();
    bool CreateFontsTexture();
    //Data members
    ImFont* fonts_[ImGui::Font::Count];

    bgfx::ProgramHandle program_ = BGFX_INVALID_HANDLE;
    bgfx::TextureHandle fontTexture_ = BGFX_INVALID_HANDLE;
    bgfx::VertexLayout vertexLayout_;
    bgfx::UniformHandle attribLocationTex_ = BGFX_INVALID_HANDLE;

	bgfx::ProgramHandle imageProgram_;
	bgfx::UniformHandle imageLodEnabled_;
    bgfx::UniformHandle samplerTex_;
};