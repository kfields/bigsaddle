#include <iostream>
#include <map>
#include <vector>
#include <filesystem>

#include <pugixml.hpp>

#include <imgui/imgui.h>
#include <bx/math.h>
#include <bgfx/utils/utils.h>

#include <examples/common.h>

using namespace bigsaddle;

class ExampleSpriteSheet : public ExampleApp {
public:
    EXAMPLE_CTOR(ExampleSpriteSheet)

    virtual void Create() override {
        ExampleApp::Create();

        texMgr_ = new TextureManager();
        texMgr_->LoadSpriteSheet("spaceshooter/sheet.xml");

        texture_ = *texMgr_->GetTexture("playerShip1_orange.png");
        sprite_ = Sprite::Produce(width() / 2, height() / 2, texture_);
    }

    virtual void Draw() override {
        ExampleApp::Draw();
        ShowExampleDialog();

        ImGui::SetNextWindowPos(
            ImVec2(width() - width() / 5.0f - 10.0f, 10.0f)
            , ImGuiCond_FirstUseEver
        );
        ImGui::SetNextWindowSize(
            ImVec2(width() / 5.0f, height() / 7.0f)
            , ImGuiCond_FirstUseEver
        );
        ImGui::Begin("Settings", NULL, 0);

        ImGui::SliderFloat("Scale", &sprite_->scale_, 0.25f, 4.0f);
        ImGui::SliderFloat("Angle", &sprite_->angle_, 0.0f, 360.0f);
        float color[4] = {
            sprite_->color_.r / 255.0f,
            sprite_->color_.g / 255.0f,
            sprite_->color_.b / 255.0f,
            sprite_->color_.a / 255.0f
        };
        if (ImGui::ColorEdit4("Color", color))
            sprite_->color_ = ColorRgba(color);

        std::vector<const char*> names = texMgr_->GetNames();
        static int item_current = texMgr_->GetNameIndex(sprite_->texture_.name);
        if (ImGui::ListBox("Textures", &item_current, names.data(), names.size(), 4)) {
            sprite_->SetTexture(*texMgr_->GetTexture(names[item_current]));
        }

        ImGui::End();

        float ortho[16];
        const bgfx::Caps* caps = bgfx::getCaps();
        bx::mtxOrtho(ortho, 0, width(), height(), 0, 0.0f, 1000.0f, 0.0f, caps->homogeneousDepth);
        bgfx::setViewTransform(viewId_, NULL, ortho);

        sprite_->Draw(viewId_);
    }
    //Data members
    Texture texture_;
    TextureManager* texMgr_ = nullptr;
    Sprite* sprite_ = nullptr;
};

EXAMPLE_MAIN(
    ExampleSpriteSheet
    , "52-spritesheet"
    , "Render sprites from spritesheet."
    , "https://bkaradzic.github.io/bgfx/examples.html#helloworld"
);

