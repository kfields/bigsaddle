#include <iostream>
#include <map>
#include <vector>
#include <filesystem>

#include <pugixml.hpp>

#include <imgui/imgui.h>
#include <bx/math.h>
#include <bgfx/utils/utils.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <examples/common.h>

using namespace bigsaddle;

class ExampleTileSet : public ExampleApp {
public:
    EXAMPLE_CTOR(ExampleTileSet)

    virtual void Create() override {
        ExampleApp::Create();

        tileset_ = new Tileset();
        tileset_->Load("platformer/tiles.tsx");

        texture_ = tileset_->GetTile(0);
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

        std::vector<const char*> names = tileset_->GetNames();
        static int item_current = sprite_->texture_.id;
        if (ImGui::ListBox("Textures", &item_current, names.data(), names.size(), 4)) {
            sprite_->SetTexture(tileset_->GetTile(item_current));
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
    Tileset* tileset_ = nullptr;
    Sprite* sprite_ = nullptr;
};

EXAMPLE_MAIN(
    ExampleTileSet
    , "54-tileset"
    , "Render tile from tileset."
    , "https://bkaradzic.github.io/bgfx/examples.html#helloworld"
);

