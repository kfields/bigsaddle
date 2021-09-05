#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct PosColorTexCoord0Vertex
{
	float m_x;
	float m_y;
	float m_z;
    ColorRgba m_abgr;
	float m_u;
	float m_v;

	static void Setup()
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
        scale_(1.0f),
        angle_(0.0f),
        texture_(Texture())
    {
    }

    Sprite(float x, float y, Texture& texture) :
        x_(x), y_(y), scale_(1.0f), angle_(0.0f),
        texture_(texture) {}

    ~Sprite() {
        if (bgfx::isValid(texture_.texture)) {
            bgfx::destroy(texture_.texture);
        }
    }
    static void Setup() {
        PosColorTexCoord0Vertex::Setup();
        program_ = loadProgram("vs_sprite", "fs_sprite");
        texture_color_ = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
        isSetup_ = true;
    }
    static Sprite* Produce(float x, float y, Texture& texture) {
        if (!isSetup_)
            Setup();
        return new Sprite(x, y, texture);
    }
    static void Shutdown() {
        if (bgfx::isValid(program_)) {
            bgfx::destroy(program_);
        }
        bgfx::destroy(texture_color_);
    }
    void Draw(uint8_t _view)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x_, y_, 0.0f));
        model = glm::scale(model, glm::vec3(scale_, scale_, 1.0f));
        model = glm::rotate(model, glm::radians(angle_), glm::vec3(0.0f, 0.0f, 1.0f));

        bgfx::TransientVertexBuffer tvb;
        bgfx::TransientIndexBuffer tib;

        if (bgfx::allocTransientBuffers(&tvb, PosColorTexCoord0Vertex::ms_layout, 4, &tib, 6))
        {
            PosColorTexCoord0Vertex* srcVert = s_quadVertices;
            PosColorTexCoord0Vertex* vertex = (PosColorTexCoord0Vertex*)tvb.data;
            float halfWidth = float(texture_.width / 2);
            float halfHeight = float(texture_.height / 2);
            for (int i = 0; i < 4; ++i) {
                glm::vec4 vert = model * glm::vec4(srcVert[i].m_x * halfWidth, srcVert[i].m_y * halfHeight, 0.0f, 1.0f);
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

    void SetTexture(Texture& tex) {
        texture_ = tex;
    }

    // Data members
    float x_;
    float y_;
    float scale_;
    float angle_;
    Texture texture_;
    ColorRgba color_;

    static bool isSetup_;
    static bgfx::ProgramHandle program_;
    static bgfx::UniformHandle texture_color_;
};
bool Sprite::isSetup_ = false;
bgfx::ProgramHandle Sprite::program_;
bgfx::UniformHandle Sprite::texture_color_;
