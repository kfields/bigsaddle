#include <imgui/imgui.h>
#include <bx/math.h>
#include <bgfx/utils/utils.h>

#include <glm/trigonometric.hpp>

#include <examples/example_app.h>

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

struct PosTexcoordVertex {
    float x;
    float y;
    float z;
    float u;
    float v;
    float w;

    static void Init() {
        ms_layout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 3, bgfx::AttribType::Float)
        .end();
    }

    static bgfx::VertexLayout ms_layout;
};
bgfx::VertexLayout PosTexcoordVertex::ms_layout;

static PosTexcoordVertex s_quadVertices[4] = {
    {-1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f },
    { 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f },
    {-1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f },
    { 1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f }
};

static const uint16_t s_quadIndices[6] = {
    0, 1, 2,
    1, 3, 2
};

class Quad {
    public:

    Quad()
        : x_(0.0f),
        y_(0.0f),
        width_(0.0f),
        height_(0.0f),
        scale_(1.0f),
        angle_(0.0f),
        texture_(Texture()) {}

    ~Quad() {
        if (bgfx::isValid(texture_.texture)) {
            bgfx::destroy(texture_.texture);
        }
        bgfx::destroy(ibh_);
        bgfx::destroy(vbh_);
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

        PosTexcoordVertex::Init();

        vbh_ = bgfx::createVertexBuffer(
            bgfx::makeRef(s_quadVertices, sizeof(s_quadVertices)),
            PosTexcoordVertex::ms_layout);

        ibh_ = bgfx::createIndexBuffer(
            bgfx::makeRef(s_quadIndices, sizeof(s_quadIndices)));

        program_ = loadProgram("vs_update", "fs_update_cmp");

        texture_color_ = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
    }

    void Quad::Draw() {
        float scaleMat[16];
        float rotateMat[16];
        float transMat[16];

        bx::mtxScale(scaleMat, width_ * scale_, height_ * scale_, 1.0f);
        bx::mtxRotateXYZ(rotateMat, 0.0f, 0.0f, glm::radians(angle_));
        bx::mtxTranslate(transMat, x_, y_, 0.0f);

        float tmpMat[16];
        float tmpMat2[16];

        bx::mtxMul(tmpMat, scaleMat, rotateMat);
        bx::mtxMul(tmpMat2, tmpMat, transMat);

        bgfx::setTransform(tmpMat2);

        bgfx::setVertexBuffer(0, vbh_);
        bgfx::setIndexBuffer(ibh_);

        bgfx::setTexture(0, texture_color_, texture_.texture);

        uint64_t state = BGFX_STATE_DEFAULT | BGFX_STATE_BLEND_ALPHA;

        bgfx::setState(state);

        bgfx::submit(0, program_);
    }
    // Data members
    float x_;
    float y_;
    float width_;
    float height_;
    float scale_;
    float angle_;
    Texture texture_;

    bgfx::ProgramHandle program_;
    bgfx::UniformHandle texture_color_;
    bgfx::IndexBufferHandle ibh_;
    bgfx::VertexBufferHandle vbh_;

};

using namespace bigsaddle;

class ExampleQuad : public ExampleApp {
public:
    EXAMPLE_CTOR(ExampleQuad)

    virtual void Create() override {
        ExampleApp::Create();
        texture_ = new Texture();
        texture_->Load("images/playerShip1_orange.png");
        sprite_ = new Quad();
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

        ImGui::End();

        float ortho[16];
        const bgfx::Caps* caps = bgfx::getCaps();
        bx::mtxOrtho(ortho, 0, width(), height(), 0, 0.0f, 1000.0f, 0.0f, caps->homogeneousDepth);
        bgfx::setViewTransform(viewId_, NULL, ortho);

        sprite_->Draw();
    }
    //Data members
    Texture* texture_ = nullptr;
    Quad* sprite_ = nullptr;
};

EXAMPLE_MAIN(
    ExampleQuad
    , "50-quad"
    , "Render a quad."
    , "https://bkaradzic.github.io/bgfx/examples.html#helloworld"
);

