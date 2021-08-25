#include <imgui/imgui.h>
//#include <imgui/imgui_internal.h>

#include <bx/bx.h>

#include <bgfx/bgfx.h>

#include "resource/roboto_regular.ttf.h"
#include "resource/robotomono_regular.ttf.h"
#include "resource/icons_kenney.ttf.h"
#include "resource/icons_font_awesome.ttf.h"

#include <iconfontheaders/icons_font_awesome.h>
#include <iconfontheaders/icons_kenney.h>

#include "gui_font.h"

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

namespace bigsaddle {

GuiFonts* GuiFonts::instance_ = nullptr;

GuiFonts& GuiFonts::instance() {
    if (instance_ == nullptr) {
        instance_ = new GuiFonts();
        instance_->Create();
    }
    return *instance_;
}

void GuiFonts::Create() {
    float _fontSize = 18.0f;

    ImGuiIO& io = ImGui::GetIO();
    ImFontConfig config;
    config.FontDataOwnedByAtlas = false;
    config.MergeMode = false;
    //config.MergeGlyphCenterV = true;

    const ImWchar* ranges = io.Fonts->GetGlyphRangesCyrillic();
    fonts_[GuiFont::Kind::Regular] = io.Fonts->AddFontFromMemoryTTF((void*)s_robotoRegularTtf, sizeof(s_robotoRegularTtf), _fontSize, &config, ranges);
    fonts_[GuiFont::Kind::Mono] = io.Fonts->AddFontFromMemoryTTF((void*)s_robotoMonoRegularTtf, sizeof(s_robotoMonoRegularTtf), _fontSize - 3.0f, &config, ranges);

    config.MergeMode = true;
    config.DstFont = fonts_[GuiFont::Kind::Regular];

    for (uint32_t ii = 0; ii < BX_COUNTOF(s_fontRangeMerge); ++ii)
    {
        const FontRangeMerge& frm = s_fontRangeMerge[ii];

        io.Fonts->AddFontFromMemoryTTF((void*)frm.data
            , (int)frm.size
            , _fontSize - 3.0f
            , &config
            , frm.ranges
        );
    }
}

bgfx::TextureHandle GuiFonts::CreateFontTexture() {
    uint8_t* data;
    int32_t width;
    int32_t height;

    ImGuiIO& io = ImGui::GetIO();
    
    io.Fonts->GetTexDataAsRGBA32(&data, &width, &height);

    bgfx::TextureHandle fontTexture = bgfx::createTexture2D(
        (uint16_t)width
        , (uint16_t)height
        , false
        , 1
        , bgfx::TextureFormat::BGRA8
        , 0
        , bgfx::copy(data, width * height * 4)
    );

    return fontTexture;
}

} //namespace bigsaddle