
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

// BGFX/BX
#include <bgfx/bgfx.h>
#include <bgfx/embedded_shader.h>
#include <bx/math.h>
#include <bx/timer.h>

#include "gui_renderer.h"

#include "resource/vs_ocornut_imgui.bin.h"
#include "resource/fs_ocornut_imgui.bin.h"
#include "resource/vs_imgui_image.bin.h"
#include "resource/fs_imgui_image.bin.h"

#include "resource/roboto_regular.ttf.h"
#include "resource/robotomono_regular.ttf.h"
#include "resource/icons_kenney.ttf.h"
#include "resource/icons_font_awesome.ttf.h"

#include <iconfontheaders/icons_font_awesome.h>
#include <iconfontheaders/icons_kenney.h>

#define IMGUI_FLAGS_ALPHA_BLEND UINT8_C(0x01)

GuiRenderer::GuiRenderer()
{
}

bool GuiRenderer::Create() {
    //ImGui::GetIO().BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
    //ImGui::GetIO().BackendRendererUserData = this;
    CreateDeviceObjects();
    return true;
}

void GuiRenderer::Shutdown()
{
    InvalidateDeviceObjects();
}

inline bool checkAvailTransientBuffers(uint32_t _numVertices, const bgfx::VertexLayout& _layout, uint32_t _numIndices)
{
	return _numVertices == bgfx::getAvailTransientVertexBuffer(_numVertices, _layout)
		&& (0 == _numIndices || _numIndices == bgfx::getAvailTransientIndexBuffer(_numIndices) )
		;
}

void GuiRenderer::Render(uint16_t viewId, ImDrawData* drawData)
{
    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
    int fb_width = (int)(drawData->DisplaySize.x * drawData->FramebufferScale.x);
    int fb_height = (int)(drawData->DisplaySize.y * drawData->FramebufferScale.y);
    if (fb_width <= 0 || fb_height <= 0) return;

    bgfx::setViewName(viewId, "ImGui");
    bgfx::setViewMode(viewId, bgfx::ViewMode::Sequential);

    const bgfx::Caps* caps = bgfx::getCaps();
    {
        float ortho[16];
        float x = drawData->DisplayPos.x;
        float y = drawData->DisplayPos.y;
        float width = drawData->DisplaySize.x;
        float height = drawData->DisplaySize.y;


        bx::mtxOrtho(ortho, x, x + width, y + height, y, 0.0f, 1000.0f, 0.0f, caps->homogeneousDepth);
        bgfx::setViewTransform(viewId, NULL, ortho);
        bgfx::setViewRect(viewId, 0, 0, uint16_t(width), uint16_t(height) );
    }

    const ImVec2 clipPos   = drawData->DisplayPos;       // (0,0) unless using multi-viewports
    const ImVec2 clipScale = drawData->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

    // Render command lists
    for (int32_t ii = 0, num = drawData->CmdListsCount; ii < num; ++ii)
    {
        bgfx::TransientVertexBuffer tvb;
        bgfx::TransientIndexBuffer tib;

        const ImDrawList* drawList = drawData->CmdLists[ii];
        uint32_t numVertices = (uint32_t)drawList->VtxBuffer.size();
        uint32_t numIndices  = (uint32_t)drawList->IdxBuffer.size();

        if (!checkAvailTransientBuffers(numVertices, vertexLayout_, numIndices) )
        {
            // not enough space in transient buffer just quit drawing the rest...
            break;
        }

        bgfx::allocTransientVertexBuffer(&tvb, numVertices, vertexLayout_);
        bgfx::allocTransientIndexBuffer(&tib, numIndices, sizeof(ImDrawIdx) == 4);

        ImDrawVert* verts = (ImDrawVert*)tvb.data;
        bx::memCopy(verts, drawList->VtxBuffer.begin(), numVertices * sizeof(ImDrawVert) );

        ImDrawIdx* indices = (ImDrawIdx*)tib.data;
        bx::memCopy(indices, drawList->IdxBuffer.begin(), numIndices * sizeof(ImDrawIdx) );

        bgfx::Encoder* encoder = bgfx::begin();

        uint32_t offset = 0;
        for (const ImDrawCmd* cmd = drawList->CmdBuffer.begin(), *cmdEnd = drawList->CmdBuffer.end(); cmd != cmdEnd; ++cmd)
        {
            if (cmd->UserCallback)
            {
                cmd->UserCallback(drawList, cmd);
            }
            else if (0 != cmd->ElemCount)
            {
                uint64_t state = 0
                    | BGFX_STATE_WRITE_RGB
                    | BGFX_STATE_WRITE_A
                    | BGFX_STATE_MSAA
                    ;

                bgfx::TextureHandle th = fontTexture_;
                bgfx::ProgramHandle program = program_;

                if (NULL != cmd->TextureId)
                {
                    union { ImTextureID ptr; struct { bgfx::TextureHandle handle; uint8_t flags; uint8_t mip; } s; } texture = { cmd->TextureId };
                    state |= 0 != (IMGUI_FLAGS_ALPHA_BLEND & texture.s.flags)
                        ? BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
                        : BGFX_STATE_NONE
                        ;
                    th = texture.s.handle;
                    if (0 != texture.s.mip)
                    {
                        const float lodEnabled[4] = { float(texture.s.mip), 1.0f, 0.0f, 0.0f };
                        bgfx::setUniform(imageLodEnabled_, lodEnabled);
                        program = imageProgram_;
                    }
                }
                else
                {
                    state |= BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);
                }

                // Project scissor/clipping rectangles into framebuffer space
                ImVec4 clipRect;
                clipRect.x = (cmd->ClipRect.x - clipPos.x) * clipScale.x;
                clipRect.y = (cmd->ClipRect.y - clipPos.y) * clipScale.y;
                clipRect.z = (cmd->ClipRect.z - clipPos.x) * clipScale.x;
                clipRect.w = (cmd->ClipRect.w - clipPos.y) * clipScale.y;

                if (clipRect.x <  fb_width
                &&  clipRect.y <  fb_height
                &&  clipRect.z >= 0.0f
                &&  clipRect.w >= 0.0f)
                {
                    const uint16_t xx = uint16_t(bx::max(clipRect.x, 0.0f) );
                    const uint16_t yy = uint16_t(bx::max(clipRect.y, 0.0f) );
                    encoder->setScissor(xx, yy
                            , uint16_t(bx::min(clipRect.z, 65535.0f)-xx)
                            , uint16_t(bx::min(clipRect.w, 65535.0f)-yy)
                            );

                    encoder->setState(state);
                    encoder->setTexture(0, samplerTex_, th);
                    encoder->setVertexBuffer(0, &tvb, 0, numVertices);
                    encoder->setIndexBuffer(&tib, offset, cmd->ElemCount);
                    encoder->submit(viewId, program);
                }
            }

            offset += cmd->ElemCount;
        }

        bgfx::end(encoder);
    }
}

struct FontRangeMerge
{
    const void* data;
    size_t      size;
    ImWchar     ranges[3];
};

static FontRangeMerge s_fontRangeMerge[] =
{
    { s_iconsKenneyTtf,      sizeof(s_iconsKenneyTtf),      { ICON_MIN_KI, ICON_MAX_KI, 0 } },
    { s_iconsFontAwesomeTtf, sizeof(s_iconsFontAwesomeTtf), { ICON_MIN_FA, ICON_MAX_FA, 0 } },
};

bool GuiRenderer::CreateFontsTexture() {
    uint8_t* data;
    int32_t width;
    int32_t height;
    float _fontSize = 18.0f;

    ImGuiIO& io = ImGui::GetIO();
    ImFontConfig config;
    config.FontDataOwnedByAtlas = true;
    config.MergeMode = false;
    //config.MergeGlyphCenterV = true;

    const ImWchar* ranges = io.Fonts->GetGlyphRangesCyrillic();
    fonts_[ImGui::Font::Regular] = io.Fonts->AddFontFromMemoryTTF( (void*)s_robotoRegularTtf,     sizeof(s_robotoRegularTtf),     _fontSize,      &config, ranges);
    fonts_[ImGui::Font::Mono   ] = io.Fonts->AddFontFromMemoryTTF( (void*)s_robotoMonoRegularTtf, sizeof(s_robotoMonoRegularTtf), _fontSize-3.0f, &config, ranges);

    config.MergeMode = true;
    config.DstFont   = fonts_[ImGui::Font::Regular];

    for (uint32_t ii = 0; ii < BX_COUNTOF(s_fontRangeMerge); ++ii)
    {
        const FontRangeMerge& frm = s_fontRangeMerge[ii];

        io.Fonts->AddFontFromMemoryTTF( (void*)frm.data
                , (int)frm.size
                , _fontSize-3.0f
                , &config
                , frm.ranges
                );
    }

    io.Fonts->GetTexDataAsRGBA32(&data, &width, &height);

    fontTexture_ = bgfx::createTexture2D(
            (uint16_t)width
        , (uint16_t)height
        , false
        , 1
        , bgfx::TextureFormat::BGRA8
        , 0
        , bgfx::copy(data, width*height*4)
        );
    return true;
}

static const bgfx::EmbeddedShader s_embeddedShaders[] = {
    BGFX_EMBEDDED_SHADER(vs_ocornut_imgui),
    BGFX_EMBEDDED_SHADER(fs_ocornut_imgui),
    BGFX_EMBEDDED_SHADER(vs_imgui_image),
    BGFX_EMBEDDED_SHADER(fs_imgui_image),

    BGFX_EMBEDDED_SHADER_END()
};

bool GuiRenderer::CreateDeviceObjects()
{
    bgfx::RendererType::Enum type = bgfx::getRendererType();
    program_ = bgfx::createProgram(
        bgfx::createEmbeddedShader(s_embeddedShaders, type, "vs_ocornut_imgui"),
        bgfx::createEmbeddedShader(s_embeddedShaders, type, "fs_ocornut_imgui"),
        true);

    vertexLayout_.begin()
        .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .end();

    attribLocationTex_ =
        bgfx::createUniform("attribLocationTex_", bgfx::UniformType::Sampler);

    imageLodEnabled_ = bgfx::createUniform("imageLodEnabled_", bgfx::UniformType::Vec4);

    imageProgram_ = bgfx::createProgram(
            bgfx::createEmbeddedShader(s_embeddedShaders, type, "vs_imgui_image")
        , bgfx::createEmbeddedShader(s_embeddedShaders, type, "fs_imgui_image")
        , true
        );

    samplerTex_ = bgfx::createUniform("s_tex", bgfx::UniformType::Sampler);

    CreateFontsTexture();

    return true;
}

void GuiRenderer::InvalidateDeviceObjects()
{
    bgfx::destroy(attribLocationTex_);
    bgfx::destroy(program_);

    if (isValid(fontTexture_)) {
        bgfx::destroy(fontTexture_);
        ImGui::GetIO().Fonts->TexID = 0;
        fontTexture_.idx = bgfx::kInvalidHandle;
    }
}
