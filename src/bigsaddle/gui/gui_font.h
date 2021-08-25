#pragma once

#include <list>

struct ImFont;

namespace bgfx {
    struct TextureHandle;
}

namespace bigsaddle {

class GuiFont {
public:
    enum Kind {
        Regular,
        Mono,
        Count
    };
};

class GuiFonts {
public:
    void Create();
    static GuiFonts& instance();
    bgfx::TextureHandle CreateFontTexture();
    // Data members
    static GuiFonts* instance_;
    ImFont* fonts_[GuiFont::Kind::Count];
};

} //namespace bigsaddle
