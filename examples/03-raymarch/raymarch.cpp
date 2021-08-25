#include <imgui/imgui.h>

#include <bx/math.h>
#include <bx/timer.h>

#include <bgfx/utils/utils.h>

#include <examples/example_app.h>


struct PosColorTexCoord0Vertex
{
	float m_x;
	float m_y;
	float m_z;
	uint32_t m_abgr;
	float m_u;
	float m_v;

	static void init()
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

void renderScreenSpaceQuad(uint8_t _view, bgfx::ProgramHandle _program, float _x, float _y, float _width, float _height)
{
	bgfx::TransientVertexBuffer tvb;
	bgfx::TransientIndexBuffer tib;

	if (bgfx::allocTransientBuffers(&tvb, PosColorTexCoord0Vertex::ms_layout, 4, &tib, 6) )
	{
		PosColorTexCoord0Vertex* vertex = (PosColorTexCoord0Vertex*)tvb.data;

		float zz = 0.0f;

		const float minx = _x;
		const float maxx = _x + _width;
		const float miny = _y;
		const float maxy = _y + _height;

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

		uint16_t* indices = (uint16_t*)tib.data;

		indices[0] = 0;
		indices[1] = 2;
		indices[2] = 1;
		indices[3] = 0;
		indices[4] = 3;
		indices[5] = 2;

		bgfx::setState(BGFX_STATE_DEFAULT);
		bgfx::setIndexBuffer(&tib);
		bgfx::setVertexBuffer(0, &tvb);
		bgfx::submit(_view, _program);
	}
}

using namespace bigsaddle;

class ExampleRaymarch : public ExampleApp {
public:
    EXAMPLE_CTOR(ExampleRaymarch)

    virtual void Create() override {
        ExampleApp::Create();
		// Create vertex stream declaration.
		PosColorTexCoord0Vertex::init();

		u_mtx          = bgfx::createUniform("u_mtx",      bgfx::UniformType::Mat4);
		u_lightDirTime = bgfx::createUniform("u_lightDirTime", bgfx::UniformType::Vec4);

		// Create program from shaders.
		m_program = loadProgram("vs_raymarching", "fs_raymarching");

		m_timeOffset = bx::getHPCounter();
    }

    virtual void Destroy() {
		bgfx::destroy(m_program);

		bgfx::destroy(u_mtx);
		bgfx::destroy(u_lightDirTime);
    }

    virtual void Draw() override {
        ExampleApp::Draw();
        ShowExampleDialog();

        // Set view 0 default viewport.
        bgfx::setViewRect(0, 0, 0, uint16_t(width()), uint16_t(height()) );

        // Set view 1 default viewport.
        bgfx::setViewRect(1, 0, 0, uint16_t(width()), uint16_t(height()) );

		// This dummy draw call is here to make sure that view 0 is cleared
		// if no other draw calls are submitted to viewZ 0.
		bgfx::touch(0);

        const bx::Vec3 at  = { 0.0f, 0.0f,   0.0f };
        const bx::Vec3 eye = { 0.0f, 0.0f, -15.0f };

        float view[16];
        float proj[16];
        bx::mtxLookAt(view, eye, at);

        const bgfx::Caps* caps = bgfx::getCaps();
        bx::mtxProj(proj, 60.0f, float(height())/float(height()), 0.1f, 100.0f, caps->homogeneousDepth);

        // Set view and projection matrix for view 1.
        bgfx::setViewTransform(0, view, proj);

        float ortho[16];
        bx::mtxOrtho(ortho, 0.0f, 1280.0f, 720.0f, 0.0f, 0.0f, 100.0f, 0.0, caps->homogeneousDepth);

        // Set view and projection matrix for view 0.
        bgfx::setViewTransform(1, NULL, ortho);

        float time = (float)( (bx::getHPCounter()-m_timeOffset)/double(bx::getHPFrequency() ) );

        float vp[16];
        bx::mtxMul(vp, view, proj);

        float mtx[16];
        bx::mtxRotateXY(mtx
            , time
            , time*0.37f
            );

        float mtxInv[16];
        bx::mtxInverse(mtxInv, mtx);
        float lightDirTime[4];
        const bx::Vec3 lightDirModelN = bx::normalize(bx::Vec3{-0.4f, -0.5f, -1.0f});
        bx::store(lightDirTime, bx::mul(lightDirModelN, mtxInv) );
        lightDirTime[3] = time;
        bgfx::setUniform(u_lightDirTime, lightDirTime);

        float mvp[16];
        bx::mtxMul(mvp, mtx, vp);

        float invMvp[16];
        bx::mtxInverse(invMvp, mvp);
        bgfx::setUniform(u_mtx, invMvp);

        renderScreenSpaceQuad(1, m_program, 0.0f, 0.0f, 1280.0f, 720.0f);
    }
    //Data members
	int64_t m_timeOffset;
	bgfx::UniformHandle u_mtx;
	bgfx::UniformHandle u_lightDirTime;
	bgfx::ProgramHandle m_program;
};

EXAMPLE_MAIN(
	  ExampleRaymarch
	, "03-raymarch"
	, "Updating shader uniforms."
	, "https://bkaradzic.github.io/bgfx/examples.html#raymarch"
	);
