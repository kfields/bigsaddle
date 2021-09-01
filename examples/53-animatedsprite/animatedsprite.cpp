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

#include <examples/example_app.h>

struct Timer {
public:
    typedef std::chrono::nanoseconds duration;
    typedef duration::rep rep;
    typedef duration::period period;
    typedef std::chrono::time_point<std::chrono::high_resolution_clock, duration> time_point;

    std::chrono::high_resolution_clock clock;
    time_point startTime;		//tick count at construction
    time_point lastElapsed;	//
    duration alarmInterval;	//
    time_point alarmTime;		//
    duration frequency;

    Timer() {
        Reset();
    }
    Timer(duration interval) {
        Reset();
        SetAlarm(interval);
    }
    time_point Now() { return clock.now(); }
    void Reset() {
        frequency = duration(1);
        startTime = Now();
        lastElapsed = startTime;
        SetAlarm(frequency);
    }
    void SetAlarm(duration interval) {
        auto time = Now();
        alarmInterval = interval;
        alarmTime = time + alarmInterval;
    }
    void ResetAlarm() {
        auto time = Now();
        alarmTime = time + alarmInterval;
    }
    void ResetAlarmMinus(duration interval) {
        auto time = Now();
        alarmTime = time + (alarmInterval - interval);
    }
    bool CheckAlarm() {
        auto time = Now();
        if (time >= alarmTime)
            return true;
        else
            return false;
    }
    long GetElapsed() {
        auto time = Now();
        long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(time - lastElapsed).count();
        lastElapsed = time;
        return elapsed;
    }
};

struct ColorRgba {
    ColorRgba() : r(255), g(255), b(255), a(255) {}
    ColorRgba(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a)
        : r(_r), g(_g), b(_b), a(_a) {}
    ColorRgba(uint32_t _c) : c(_c) {}
    ColorRgba(float _r, float _g, float _b, float _a)
        : r(_r * 255.0f), g(_g * 255.0f), b(_b * 255.0f), a(_a * 255.0f) {}
    ColorRgba(const float* arr)
        : ColorRgba(arr[0], arr[1], arr[2], arr[3]) {}

    union {
        struct {
            uint8_t r;
            uint8_t g;
            uint8_t b;
            uint8_t a;
        };
        uint32_t c;
    };
};

struct TexCoord {
    TexCoord(float _u, float _v) : u(_u), v(_v) {}
    float u;
    float v;
};

struct Texture {
    Texture() {}
    Texture(const Texture& that) {
        name = that.name;
        texture = that.texture;
        x = that.x;
        y = that.y;
        width = that.width;
        height = that.height;
        coords[0] = that.coords[0];
        coords[1] = that.coords[1];
        coords[2] = that.coords[2];
        coords[3] = that.coords[3];
    }
    Texture(Texture* _parent, std::string _name, int _x, int _y, int _width, int _height) :
        parent(_parent), name(_name), texture(_parent->texture), x(_x), y(_y), width(_width), height(_height) {
        float pWidth = parent->width;
        float hWidth = parent->width / 2;
        float pHeight = parent->height;
        float hHeight = parent->height / 2;
        coords[0] = TexCoord(x / pWidth, (y + height) / pHeight);
        coords[1] = TexCoord((x + width) / pWidth, (y + height) / pHeight);
        coords[2] = TexCoord(x / pWidth, y / pHeight);
        coords[3] = TexCoord((x + width) / pWidth, y / pHeight);
    }
    Texture Mirror() {
        Texture texture(*this);
        texture.coords[0].u = coords[1].u;
        texture.coords[1].u = coords[0].u;
        texture.coords[2].u = coords[3].u;
        texture.coords[3].u = coords[2].u;
        return texture;
    }
    void Load(const std::filesystem::path& _path) {
        bgfx::TextureInfo info = bgfx::TextureInfo();

        texture = loadTexture(_path.string().c_str(), BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT, 0, &info);
        if (!bgfx::isValid(texture)) {
            //TODO: throw
        }

        width = info.width;
        height = info.height;
        name = _path.string();
    }
    Texture* parent;
    std::string name;
    bgfx::TextureHandle texture;
    float x;
    float y;
    float width;
    float height;
    TexCoord coords[4] = {
        {0.0f,  1.0f},
        {1.0f,  1.0f},
        {0.0f, 0.0f},
        {1.0f, 0.0f}
    };

};

class TextureManager {
public:
    int LoadSpriteSheet(const std::filesystem::path& _path) {
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(_path.c_str());
        if (!result)
            return -1;
        
        pugi::xml_node atlas = doc.child("TextureAtlas");
        const char* imagePath = atlas.attribute("imagePath").as_string();

        Texture* texture = new Texture();
        texture->Load(_path.parent_path() / imagePath);

        for (pugi::xml_node node: atlas.children("SubTexture"))
        {
            //<SubTexture name="beam0.png" x="143" y="377" width="43" height="31"/>
            std::string name = node.attribute("name").as_string();
            int x = node.attribute("x").as_int();
            int y = node.attribute("y").as_int();
            int width = node.attribute("width").as_int();
            int height = node.attribute("height").as_int();
            //std::cout << name << x << y << width << height << "\n";
            Texture* subTex = new Texture(texture, name, x, y, width, height);
            textures_[name] = subTex;
        }
    }
    Texture* GetTexture(const std::string& _name) {
        return textures_[_name];
    }
    std::vector<const char*> GetNames() {
        std::vector<const char*> names;
        for (std::map<std::string, Texture*>::iterator it = textures_.begin(); it != textures_.end(); ++it) {
            names.push_back(it->first.c_str());
        }
        return names;
    }
    int GetNameIndex(const std::string& _name) {
        return std::distance(textures_.begin(), textures_.find(_name));
    }
    // Data members
    std::map<std::string, Texture*> textures_;
};

struct PosColorTexCoord0Vertex
{
	float m_x;
	float m_y;
	float m_z;
    ColorRgba m_abgr;
	float m_u;
	float m_v;

	static void Init()
	{
		ms_layout
			.begin()
			.add(bgfx::Attrib::Position,  3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0,    4, bgfx::AttribType::Uint8, true)
			.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
			.end();
	}

	static bgfx::VertexLayout ms_layout;
};
bgfx::VertexLayout PosColorTexCoord0Vertex::ms_layout;

static PosColorTexCoord0Vertex s_quadVertices[4] = {
    {-1.0f,  1.0f,  1.0f, 0xff000000, 0.0f,  1.0f},
    { 1.0f,  1.0f,  1.0f, 0xff000000,  1.0f,  1.0f},
    {-1.0f, -1.0f,  1.0f, 0xff000000, 0.0f, 0.0f},
    { 1.0f, -1.0f,  1.0f, 0xff000000,  1.0f, 0.0f}
};

class Sprite {
    public:

        Sprite()
            : x_(0.0f),
            y_(0.0f),
            width_(0.0f),
            height_(0.0f),
            scale_(1.0f),
            angle_(0.0f),
            texture_(Texture())
        {
        }

    ~Sprite() {
        if (bgfx::isValid(texture_.texture)) {
            bgfx::destroy(texture_.texture);
        }
        if (bgfx::isValid(program_)) {
            bgfx::destroy(program_);
        }
        bgfx::destroy(texture_color_);
    }

    void Init(float x, float y, float width, float height, Texture* texture) {
        x_ = x;
        y_ = y;
        width_ = width;
        height_ = height;

        texture_ = *texture;

        PosColorTexCoord0Vertex::Init();

        program_ = loadProgram("vs_sprite", "fs_sprite");

        texture_color_ = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
    }

    void Draw(uint8_t _view)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x_, y_, 0.0f));
        model = glm::scale(model, glm::vec3(scale_, scale_, 1.0f));
        model = glm::rotate(model, glm::radians(angle_), glm::vec3(0.0f, 0.0f, 1.0f));

        bgfx::TransientVertexBuffer tvb;
        bgfx::TransientIndexBuffer tib;

        if (bgfx::allocTransientBuffers(&tvb, PosColorTexCoord0Vertex::ms_layout, 4, &tib, 6) )
        {
            PosColorTexCoord0Vertex* srcVert = s_quadVertices;
            PosColorTexCoord0Vertex* vertex = (PosColorTexCoord0Vertex*)tvb.data;
            float halfWidth = width_ / 2;
            float halfHeight = height_ / 2;
            for (int i = 0; i < 4; ++i) {
                glm::vec4 vert = model * glm::vec4(srcVert[i].m_x*halfWidth, srcVert[i].m_y*halfHeight, 0.0f, 1.0f);
                vertex[i].m_x = vert[0];
                vertex[i].m_y = vert[1];
                vertex[i].m_z = vert[2];
                vertex[i].m_abgr = color_;
                vertex[i].m_u = texture_.coords[i].u;
                vertex[i].m_v = texture_.coords[i].v;
            }

            uint16_t* indices = (uint16_t*)tib.data;

            indices[0] = 0;
            indices[1] = 1;
            indices[2] = 2;
            indices[3] = 1;
            indices[4] = 3;
            indices[5] = 2;

            bgfx::setIndexBuffer(&tib);
            bgfx::setVertexBuffer(0, &tvb);

            bgfx::setTexture(0, texture_color_, texture_.texture);

            uint64_t state = BGFX_STATE_DEFAULT | BGFX_STATE_BLEND_ALPHA;

            bgfx::setState(state);

            bgfx::submit(_view, program_);
        }
    }

    void SetTexture(Texture* tex) {
        texture_ = *tex;
        width_ = tex->width;
        height_ = tex->height;
    }

    // Data members
    float x_;
    float y_;
    float width_;
    float height_;
    float scale_;
    float angle_;
    Texture texture_;
    ColorRgba color_;

    bgfx::ProgramHandle program_;
    bgfx::UniformHandle texture_color_;
};

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

        texture_ = texMgr_->GetTexture("idle");
        sprite_ = new AnimatedSprite();
        sprite_->Init(width() / 2, height() / 2, texture_->width, texture_->height, texture_);

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
    Texture* texture_ = nullptr;
    TextureManager* texMgr_ = nullptr;
    AnimatedSprite* sprite_ = nullptr;
};

EXAMPLE_MAIN(
    ExampleAnimatedSprite
    , "53-animatedsprite"
    , "Animated Sprite."
    , "https://bkaradzic.github.io/bgfx/examples.html#helloworld"
);

