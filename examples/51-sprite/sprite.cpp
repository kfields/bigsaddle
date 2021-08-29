#include <imgui/imgui.h>
#include <bx/math.h>
#include <bgfx/utils/utils.h>

//#include <glm/glm.hpp>
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

struct Texture {
    void Load(const std::string& _path) {
        bgfx::TextureInfo info = bgfx::TextureInfo();

        texture = loadTexture(_path.data(), BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT, 0, &info);
        if (!bgfx::isValid(texture)) {
            //TODO: throw
        }

        width = info.width;
        height = info.height;
        path = _path;

    }
    std::string path;
    bgfx::TextureHandle texture;
    int width;
    int height;
};

struct PosColorTexCoord0Vertex
{
	float m_x;
	float m_y;
	float m_z;
    ColorRgba m_abgr;
	float m_u;
	float m_v;
    float m_w;

	static void Init()
	{
		ms_layout
			.begin()
			.add(bgfx::Attrib::Position,  3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0,    4, bgfx::AttribType::Uint8, true)
			.add(bgfx::Attrib::TexCoord0, 3, bgfx::AttribType::Float)
			.end();
	}

	static bgfx::VertexLayout ms_layout;
};
bgfx::VertexLayout PosColorTexCoord0Vertex::ms_layout;

static PosColorTexCoord0Vertex s_quadVertices[4] = {
    {-1.0f,  1.0f,  1.0f, 0xff000000, -1.0f,  1.0f,  1.0f },
    { 1.0f,  1.0f,  1.0f, 0xff000000,  1.0f,  1.0f,  1.0f },
    {-1.0f, -1.0f,  1.0f, 0xff000000, -1.0f, -1.0f,  1.0f },
    { 1.0f, -1.0f,  1.0f, 0xff000000,  1.0f, -1.0f,  1.0f }
};

static const uint16_t s_quadIndices[6] = {
    0, 1, 2,
    1, 3, 2
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
                vertex[i].m_u = srcVert[i].m_u;
                vertex[i].m_v = srcVert[i].m_v;
                vertex[i].m_w = srcVert[i].m_w;
            }
            float zz = 0.0f;

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

class ExampleSprite : public ExampleApp {
public:
    EXAMPLE_CTOR(ExampleSprite)

    virtual void Create() override {
        ExampleApp::Create();
        texture_ = new Texture();
        texture_->Load("images/playerShip1_orange.png");
        sprite_ = new Sprite();
        sprite_->Init(width()/2, height()/2, texture_->width, texture_->height, texture_);
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

        ImGui::End();

        float ortho[16];
        const bgfx::Caps* caps = bgfx::getCaps();
        bx::mtxOrtho(ortho, 0, width(), height(), 0, 0.0f, 1000.0f, 0.0f, caps->homogeneousDepth);
        bgfx::setViewTransform(viewId_, NULL, ortho);

        sprite_->Draw(viewId_);
    }
    //Data members
    Texture* texture_ = nullptr;
    Sprite* sprite_ = nullptr;
};

EXAMPLE_MAIN(
    ExampleSprite
    , "50-sprite"
    , "Render a sprite."
    , "https://bkaradzic.github.io/bgfx/examples.html#helloworld"
);

