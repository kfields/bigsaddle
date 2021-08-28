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

struct PosColorTexCoord0Vertex
{
	float m_x;
	float m_y;
	float m_z;
	uint32_t m_abgr;
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

/*static PosTexcoordVertex s_quadVertices[4] = {
    {-1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f },
    { 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f },
    {-1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f },
    { 1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f }
};*/

static const uint16_t s_quadIndices[6] = {
    0, 2, 1,  // 0
    0, 3, 2
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
        texture_(Texture()) {}

    ~Sprite() {
        if (bgfx::isValid(texture_.texture)) {
            bgfx::destroy(texture_.texture);
        }
        bgfx::destroy(ibh_);
        //bgfx::destroy(vbh_);
        if (bgfx::isValid(program_)) {
            bgfx::destroy(program_);
        }
        bgfx::destroy(texture_color_);
    }

    void SetPosition(float x, float y) {
        x_ = x;
        y_ = y;
    }

    void SetDimensions(float width, float height) {
        width_ = width;
        height_ = height;
    }

    void Init(float x, float y, float width, float height, Texture* texture) {
        x_ = x;
        y_ = y;
        width_ = width;
        height_ = height;

        texture_ = *texture;

        PosColorTexCoord0Vertex::Init();

        /*vbh_ = bgfx::createVertexBuffer(
            bgfx::makeRef(s_quadVertices, sizeof(s_quadVertices)),
            PosTexcoordVertex::ms_layout);*/

        ibh_ = bgfx::createIndexBuffer(
            bgfx::makeRef(s_quadIndices, sizeof(s_quadIndices)));

        program_ = loadProgram("vs_sprite", "fs_sprite");

        texture_color_ = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
    }

    void Draw(uint8_t _view)
    {
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

        bgfx::TransientVertexBuffer tvb;
        //bgfx::TransientIndexBuffer tib;

        //if (bgfx::allocTransientBuffers(&tvb, PosColorTexCoord0Vertex::ms_layout, 4, &tib, 6) )
        bgfx::allocTransientVertexBuffer(&tvb, 4, PosColorTexCoord0Vertex::ms_layout);
        {
            PosColorTexCoord0Vertex* vertex = (PosColorTexCoord0Vertex*)tvb.data;

            float zz = 0.0f;

            const float minx = -1.0f;
            const float maxx = 1.0f;
            const float miny = -1.0f;
            const float maxy = 1.0f;

            float minu = -1.0f;
            float minv = -1.0f;
            float maxu =  1.0f;
            float maxv =  1.0f;

            vertex[0].m_x = minx;
            vertex[0].m_y = miny;
            vertex[0].m_z = zz;
            vertex[0].m_abgr = 0xff0000ff;
            vertex[0].m_u = minu;
            vertex[0].m_v = minv;

            vertex[1].m_x = maxx;
            vertex[1].m_y = miny;
            vertex[1].m_z = zz;
            vertex[1].m_abgr = 0xff00ff00;
            vertex[1].m_u = maxu;
            vertex[1].m_v = minv;

            vertex[2].m_x = maxx;
            vertex[2].m_y = maxy;
            vertex[2].m_z = zz;
            vertex[2].m_abgr = 0xffff0000;
            vertex[2].m_u = maxu;
            vertex[2].m_v = maxv;

            vertex[3].m_x = minx;
            vertex[3].m_y = maxy;
            vertex[3].m_z = zz;
            vertex[3].m_abgr = 0xffffffff;
            vertex[3].m_u = minu;
            vertex[3].m_v = maxv;

            /*uint16_t* indices = (uint16_t*)tib.data;

            indices[0] = 0;
            indices[1] = 2;
            indices[2] = 1;
            indices[3] = 0;
            indices[4] = 3;
            indices[5] = 2;*/

            //bgfx::setIndexBuffer(&tib);
            bgfx::setIndexBuffer(ibh_);
            bgfx::setVertexBuffer(0, &tvb);

            bgfx::setTexture(0, texture_color_, texture_.texture);

            uint64_t state = BGFX_STATE_DEFAULT | BGFX_STATE_BLEND_ALPHA;

            bgfx::setState(state);

            bgfx::submit(_view, program_);
        }
    }

    /*void Sprite::Draw() {
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
    }*/
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
    //bgfx::VertexBufferHandle vbh_;

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

