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

#include <examples/example_app.h>

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
    Texture(Texture* _parent, std::string _name, int _x, int _y, int _width, int _height) :
        parent(_parent), name(_name), texture(_parent->texture), x(_x), y(_y), width(_width), height(_height) {
        float pWidth = parent->width;
        float pHeight = parent->height;
        coords[0] = TexCoord(x / pWidth, (y + height) / pHeight);
        coords[1] = TexCoord((x + width) / pWidth, (y + height) / pHeight);
        coords[2] = TexCoord(x / pWidth, y / pHeight);
        coords[3] = TexCoord((x + width) / pWidth, y / pHeight);
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
    std::string name;
    bgfx::TextureHandle texture;
    float x;
    float y;
    float width;
    float height;
    Texture* parent;
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

        for (pugi::xml_node subTex: atlas.children("SubTexture"))
        {
            //<SubTexture name="beam0.png" x="143" y="377" width="43" height="31"/>
            std::string name = subTex.attribute("name").as_string();
            int x = subTex.attribute("x").as_int();
            int y = subTex.attribute("y").as_int();
            int width = subTex.attribute("width").as_int();
            int height = subTex.attribute("height").as_int();
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

using namespace bigsaddle;

class ExampleSpriteSheet : public ExampleApp {
public:
    EXAMPLE_CTOR(ExampleSpriteSheet)

    virtual void Create() override {
        ExampleApp::Create();

        texMgr_ = new TextureManager();
        texMgr_->LoadSpriteSheet("spaceshooter/sheet.xml");

        texture_ = texMgr_->GetTexture("playerShip1_orange.png");
        sprite_ = new Sprite();
        sprite_->Init(width() / 2, height() / 2, texture_->width, texture_->height, texture_);
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
            sprite_->SetTexture(texMgr_->GetTexture(names[item_current]));
        }

        ImGui::End();

        float ortho[16];
        const bgfx::Caps* caps = bgfx::getCaps();
        bx::mtxOrtho(ortho, 0, width(), height(), 0, 0.0f, 1000.0f, 0.0f, caps->homogeneousDepth);
        bgfx::setViewTransform(viewId_, NULL, ortho);

        sprite_->Draw(viewId_);
    }
    //Data members
    Texture* texture_ = nullptr;
    TextureManager* texMgr_ = nullptr;
    Sprite* sprite_ = nullptr;
};

EXAMPLE_MAIN(
    ExampleSpriteSheet
    , "52-spritesheet"
    , "Render sprites from spritesheet."
    , "https://bkaradzic.github.io/bgfx/examples.html#helloworld"
);

