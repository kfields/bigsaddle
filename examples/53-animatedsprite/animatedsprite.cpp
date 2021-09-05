#include <iostream>
#include <map>
#include <vector>
#include <filesystem>

#include <fmt/core.h>
#include <pugixml.hpp>

#include <imgui/imgui.h>
#include <bx/math.h>
#include <bx/timer.h>
#include <bgfx/utils/utils.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <examples/common.h>

struct SpriteAnimation {
    SpriteAnimation() {}
    SpriteAnimation(const SpriteAnimation& that) {
        name = that.name;
        frames = that.frames;
    }
    SpriteAnimation(std::string _name) : name(_name) {}
    SpriteAnimation* Mirror(std::string _name) {
        SpriteAnimation& animation = *new SpriteAnimation(*this);
        animation.name = _name;
        for (int i = 0; i < frames.size(); ++i) {
            animation.frames[i] = frames[i].Mirror();
        }
        return &animation;
    }
    void AddFrame(Texture* texture) {
        frames.push_back(*texture);
    }
    Texture GetFrame(int index) {
        return frames[index];
    }
    int Count() {
        return frames.size();
    }
    // Data members
    std::string name;
    std::vector<Texture> frames;
};

struct SpriteAnimations {
    void AddAnimation(SpriteAnimation* _animation) {
        animations[_animation->name] = _animation;
    }
    SpriteAnimation* GetAnimation(std::string name) {
        if (name == "default") {
            return animations.begin()->second;
        }
        return animations[name];
    }
    std::vector<const char*> GetNames() {
        std::vector<const char*> names;
        for (std::map<std::string, SpriteAnimation*>::iterator it = animations.begin(); it != animations.end(); ++it) {
            names.push_back(it->first.c_str());
        }
        return names;
    }
    int GetNameIndex(const std::string& _name) {
        return std::distance(animations.begin(), animations.find(_name));
    }
    // Data members
    std::map<std::string, SpriteAnimation*> animations;
};

struct AnimatedSprite : Sprite {
    AnimatedSprite() : Sprite() {
        SetFps(10);
    }
    AnimatedSprite(float x, float y, Texture& texture) : Sprite(x, y, texture) {
        SetFps(10);
    }

    static AnimatedSprite* Produce(float x, float y, Texture& texture) {
        if (!isSetup_)
            Setup();
        return new AnimatedSprite(x, y, texture);
    }
    void Update() {
        if (timer.CheckAlarm()) {
            index = index + 1 > animation.Count() -1 ? 0 : index + 1;
            texture_ = animation.GetFrame(index);
            timer.ResetAlarm();
        }
    }
    void SetAnimation(SpriteAnimation* _animation) {
        animation = *_animation;
        texture_ = animation.GetFrame(0);
    }
    void SetAnimation(std::string name) {
        SetAnimation(animations.animations[name]);
    }
    void SetAnimations(SpriteAnimations* _animations) {
        animations = *_animations;
        SetAnimation(animations.GetAnimation("default"));
    }
    void SetFps(int _fps) {
        fps = _fps == 0 ? 1 : _fps;
        UpdateFps();
    }
    void UpdateFps() {
        timer.SetAlarm(std::chrono::milliseconds(1000 / fps));
    }
    // Data members
    SpriteAnimations animations;
    SpriteAnimation animation;
    int index = 0;
    int fps = 10; //animation frames per second
    Timer timer;
};

using namespace bigsaddle;

class ExampleAnimatedSprite : public ExampleApp {
public:
    EXAMPLE_CTOR(ExampleAnimatedSprite)

    virtual void Create() override {
        ExampleApp::Create();

        texMgr_ = new TextureManager();
        texMgr_->LoadSpriteSheet("characters/robot/sheet.xml");

        texture_ = *texMgr_->GetTexture("idle");
        sprite_ = AnimatedSprite::Produce(width() / 2, height() / 2, texture_);

        SpriteAnimations* animations = new SpriteAnimations();

        SpriteAnimation* walk = new SpriteAnimation("walkRight");
        for (int i = 0; i < 8; ++i) {
            walk->AddFrame(texMgr_->GetTexture(fmt::format("walk{}", i)));
        }
        animations->AddAnimation(walk);
        animations->AddAnimation(walk->Mirror("walkLeft"));

        SpriteAnimation* run = new SpriteAnimation("runRight");
        for (int i = 0; i < 3; ++i) {
            run->AddFrame(texMgr_->GetTexture(fmt::format("run{}", i)));
        }
        animations->AddAnimation(run);
        animations->AddAnimation(run->Mirror("runLeft"));

        sprite_->SetAnimations(animations);
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

        std::vector<const char*> names = sprite_->animations.GetNames();
        static int item_current = sprite_->animations.GetNameIndex(sprite_->animation.name);
        if (ImGui::ListBox("Animations", &item_current, names.data(), names.size(), 4)) {
            sprite_->SetAnimation(sprite_->animations.GetAnimation(names[item_current]));
        }

        if (ImGui::SliderInt("Fps", &sprite_->fps, 1, 60)) {
            sprite_->UpdateFps();
        }

        ImGui::End();

        float ortho[16];
        const bgfx::Caps* caps = bgfx::getCaps();
        bx::mtxOrtho(ortho, 0, width(), height(), 0, 0.0f, 1000.0f, 0.0f, caps->homogeneousDepth);
        bgfx::setViewTransform(viewId_, NULL, ortho);

        sprite_->Update();
        sprite_->Draw(viewId_);
    }
    //Data members
    Texture texture_;
    TextureManager* texMgr_ = nullptr;
    AnimatedSprite* sprite_ = nullptr;
};

EXAMPLE_MAIN(
    ExampleAnimatedSprite
    , "53-animatedsprite"
    , "Animated Sprite."
    , "https://bkaradzic.github.io/bgfx/examples.html#helloworld"
);

