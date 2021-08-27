#include <imgui/imgui.h>
#include <bx/math.h>
#include <bx/timer.h>

#include <bgfx/embedded_shader.h>

#include <examples/example_app.h>

// embedded shaders
#include "vs_metaballs.bin.h"
#include "fs_metaballs.bin.h"

// data
#include "data.h"

static const bgfx::EmbeddedShader s_embeddedShaders[] =
{
	BGFX_EMBEDDED_SHADER(vs_metaballs),
	BGFX_EMBEDDED_SHADER(fs_metaballs),

	BGFX_EMBEDDED_SHADER_END()
};

struct PosNormalColorVertex
{
	float m_pos[3];
	float m_normal[3];
	uint32_t m_abgr;

	static void init()
	{
		ms_layout
			.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Normal,   3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
			.end();
	};

	static bgfx::VertexLayout ms_layout;
};

bgfx::VertexLayout PosNormalColorVertex::ms_layout;

struct Grid
{
	Grid()
		: m_val(0.0f)
	{
		m_normal[0] = 0.0f;
		m_normal[1] = 0.0f;
		m_normal[2] = 0.0f;
	}

	float m_val;
	float m_normal[3];
};

float vertLerp(float* _result, float _iso, uint32_t _idx0, float _v0, uint32_t _idx1, float _v1)
{
	const float* edge0 = s_cube[_idx0];
	const float* edge1 = s_cube[_idx1];

	if (bx::abs(_iso-_v1) < 0.00001f)
	{
		_result[0] = edge1[0];
		_result[1] = edge1[1];
		_result[2] = edge1[2];
		return 1.0f;
	}

	if (bx::abs(_iso-_v0) < 0.00001f
	||  bx::abs(_v0-_v1) < 0.00001f)
	{
		_result[0] = edge0[0];
		_result[1] = edge0[1];
		_result[2] = edge0[2];
		return 0.0f;
	}

	float lerp = (_iso - _v0) / (_v1 - _v0);
	_result[0] = edge0[0] + lerp * (edge1[0] - edge0[0]);
	_result[1] = edge0[1] + lerp * (edge1[1] - edge0[1]);
	_result[2] = edge0[2] + lerp * (edge1[2] - edge0[2]);

	return lerp;
}

uint32_t triangulate(
	  uint8_t* _result
	, uint32_t _stride
	, const float* _rgb
	, const float* _xyz
	, const Grid* _val[8]
	, float _iso
	, float _scale
	)
{
	uint8_t cubeindex = 0;
	cubeindex |= (_val[0]->m_val < _iso) ? 0x01 : 0;
	cubeindex |= (_val[1]->m_val < _iso) ? 0x02 : 0;
	cubeindex |= (_val[2]->m_val < _iso) ? 0x04 : 0;
	cubeindex |= (_val[3]->m_val < _iso) ? 0x08 : 0;
	cubeindex |= (_val[4]->m_val < _iso) ? 0x10 : 0;
	cubeindex |= (_val[5]->m_val < _iso) ? 0x20 : 0;
	cubeindex |= (_val[6]->m_val < _iso) ? 0x40 : 0;
	cubeindex |= (_val[7]->m_val < _iso) ? 0x80 : 0;

	if (0 == s_edges[cubeindex])
	{
		return 0;
	}

	float verts[12][6];
	uint16_t flags = s_edges[cubeindex];

	for (uint32_t ii = 0; ii < 12; ++ii)
	{
		if (flags & (1<<ii) )
		{
			uint32_t idx0 = ii&7;
			uint32_t idx1 = "\x1\x2\x3\x0\x5\x6\x7\x4\x4\x5\x6\x7"[ii];
			float* vertex = verts[ii];
			float lerp = vertLerp(vertex, _iso, idx0, _val[idx0]->m_val, idx1, _val[idx1]->m_val);

			const float* na = _val[idx0]->m_normal;
			const float* nb = _val[idx1]->m_normal;
			vertex[3] = na[0] + lerp * (nb[0] - na[0]);
			vertex[4] = na[1] + lerp * (nb[1] - na[1]);
			vertex[5] = na[2] + lerp * (nb[2] - na[2]);
		}
	}

	const float dr = _rgb[3] - _rgb[0];
	const float dg = _rgb[4] - _rgb[1];
	const float db = _rgb[5] - _rgb[2];

	uint32_t num = 0;
	const int8_t* indices = s_indices[cubeindex];
	for (uint32_t ii = 0; indices[ii] != -1; ++ii)
	{
		const float* vertex = verts[uint8_t(indices[ii])];

		float* xyz = (float*)_result;
		xyz[0] = _xyz[0] + vertex[0]*_scale;
		xyz[1] = _xyz[1] + vertex[1]*_scale;
		xyz[2] = _xyz[2] + vertex[2]*_scale;

		xyz[3] = vertex[3]*_scale;
		xyz[4] = vertex[4]*_scale;
		xyz[5] = vertex[5]*_scale;

		uint32_t rr = uint8_t( (_rgb[0] + vertex[0]*dr)*255.0f);
		uint32_t gg = uint8_t( (_rgb[1] + vertex[1]*dg)*255.0f);
		uint32_t bb = uint8_t( (_rgb[2] + vertex[2]*db)*255.0f);

		uint32_t* abgr = (uint32_t*)&_result[24];
		*abgr = 0xff000000
			  | (bb<<16)
			  | (gg<<8)
			  | rr
			  ;

		_result += _stride;
		++num;
	}

	return num;
}

constexpr uint32_t kMaxDims  = 32;
constexpr float    kMaxDimsF = float(kMaxDims);

using namespace bigsaddle;

class ExampleMetaballs : public ExampleApp {
public:
    EXAMPLE_CTOR(ExampleMetaballs)
    virtual void Create() override {
        ExampleApp::Create();
		// Create vertex stream declaration.
		PosNormalColorVertex::init();

		bgfx::RendererType::Enum type = bgfx::getRendererType();

		bgfx::ShaderHandle vsh = bgfx::createEmbeddedShader(s_embeddedShaders, type, "vs_metaballs");
		bgfx::ShaderHandle fsh = bgfx::createEmbeddedShader(s_embeddedShaders, type, "fs_metaballs");

		// Create program from shaders.
		program_ = bgfx::createProgram(vsh, fsh, true /* destroy shaders when program is destroyed */);

		grid_ = new Grid[kMaxDims*kMaxDims*kMaxDims];
		timeOffset_ = bx::getHPCounter();
    }

    virtual void Draw() override {
        ExampleApp::Draw();
        ShowExampleDialog();

		const uint32_t ypitch = kMaxDims;
		const uint32_t zpitch = kMaxDims*kMaxDims;

        ImGui::SetNextWindowPos(
                ImVec2(width() - width() / 5.0f - 10.0f, 10.0f)
            , ImGuiCond_FirstUseEver
            );
        ImGui::SetNextWindowSize(
                ImVec2(width() / 5.0f, height() / 7.0f)
            , ImGuiCond_FirstUseEver
            );
        ImGui::Begin("Settings"
            , NULL
            , 0
            );
        static float iso = 0.75f;
        ImGui::SliderFloat("ISO", &iso, 0.1f, 4.0f);

        static uint32_t numDims = kMaxDims;
        ImGui::SliderInt("Size", (int*)&numDims, 8, kMaxDims);
        ImGui::End();

        const float numDimsF = float(numDims);
        const float scale    = kMaxDimsF/numDimsF;
        const float invDim   = 1.0f/(numDimsF-1.0f);

        // Set view 0 default viewport.
        bgfx::setViewRect(viewId_, 0, 0, uint16_t(width()), uint16_t(height()) );

        // This dummy draw call is here to make sure that view 0 is cleared
        // if no other draw calls are submitted to view 0.
        bgfx::touch(viewId_);

        int64_t now = bx::getHPCounter();
        static int64_t last = now;
        const int64_t frameTime = now - last;
        last = now;
        const double freq = double(bx::getHPFrequency() );
        const double toMs = 1000.0/freq;
        float time = (float)( (now - timeOffset_)/double(bx::getHPFrequency() ) );

        const bx::Vec3 at  = { 0.0f, 0.0f,   0.0f };
        const bx::Vec3 eye = { 0.0f, 0.0f, -50.0f };

        // Set view and projection matrix for view 0.
        {
            float view[16];
            bx::mtxLookAt(view, eye, at);

            float proj[16];
            bx::mtxProj(proj, 60.0f, float(width())/float(height()), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
            bgfx::setViewTransform(0, view, proj);

            // Set view 0 default viewport.
            bgfx::setViewRect(0, 0, 0, uint16_t(width()), uint16_t(height()) );
        }

        // Stats.
        uint32_t numVertices = 0;
        int64_t profUpdate = 0;
        int64_t profNormal = 0;
        int64_t profTriangulate = 0;

        // Allocate 32K vertices in transient vertex buffer.
        uint32_t maxVertices = (32<<10);
        bgfx::TransientVertexBuffer tvb;
        bgfx::allocTransientVertexBuffer(&tvb, maxVertices, PosNormalColorVertex::ms_layout);

        const uint32_t numSpheres = 16;
        float sphere[numSpheres][4];
        for (uint32_t ii = 0; ii < numSpheres; ++ii)
        {
            sphere[ii][0] = bx::sin(time*(ii*0.21f)+ii*0.37f) * (kMaxDimsF * 0.5f - 8.0f);
            sphere[ii][1] = bx::sin(time*(ii*0.37f)+ii*0.67f) * (kMaxDimsF * 0.5f - 8.0f);
            sphere[ii][2] = bx::cos(time*(ii*0.11f)+ii*0.13f) * (kMaxDimsF * 0.5f - 8.0f);
            sphere[ii][3] = 1.0f/(3.0f + (bx::sin(time*(ii*0.13f) )*0.5f+0.5f)*0.9f );
        }

        profUpdate = bx::getHPCounter();

        for (uint32_t zz = 0; zz < numDims; ++zz)
        {
            for (uint32_t yy = 0; yy < numDims; ++yy)
            {
                uint32_t offset = (zz*kMaxDims+yy)*kMaxDims;

                for (uint32_t xx = 0; xx < numDims; ++xx)
                {
                    uint32_t xoffset = offset + xx;

                    float dist = 0.0f;
                    float prod = 1.0f;
                    for (uint32_t ii = 0; ii < numSpheres; ++ii)
                    {
                        const float* pos = sphere[ii];
                        float dx   = pos[0] - (-kMaxDimsF*0.5f + float(xx)*scale);
                        float dy   = pos[1] - (-kMaxDimsF*0.5f + float(yy)*scale);
                        float dz   = pos[2] - (-kMaxDimsF*0.5f + float(zz)*scale);
                        float invR = pos[3];
                        float dot  = dx*dx + dy*dy + dz*dz;
                        dot *= bx::square(invR);

                        dist *= dot;
                        dist += prod;
                        prod *= dot;
                    }

                    grid_[xoffset].m_val = dist / prod - 1.0f;
                }
            }
        }

        profUpdate = bx::getHPCounter() - profUpdate;

        profNormal = bx::getHPCounter();

        for (uint32_t zz = 1; zz < numDims-1; ++zz)
        {
            for (uint32_t yy = 1; yy < numDims-1; ++yy)
            {
                uint32_t offset = (zz*kMaxDims+yy)*kMaxDims;

                for (uint32_t xx = 1; xx < numDims-1; ++xx)
                {
                    uint32_t xoffset = offset + xx;

                    Grid* grid = grid_;
                    const bx::Vec3 normal =
                    {
                        grid[xoffset-1     ].m_val - grid[xoffset+1     ].m_val,
                        grid[xoffset-ypitch].m_val - grid[xoffset+ypitch].m_val,
                        grid[xoffset-zpitch].m_val - grid[xoffset+zpitch].m_val,
                    };

                    bx::store(grid[xoffset].m_normal, bx::normalize(normal) );
                }
            }
        }

        profNormal = bx::getHPCounter() - profNormal;

        profTriangulate = bx::getHPCounter();

        PosNormalColorVertex* vertex = (PosNormalColorVertex*)tvb.data;

        for (uint32_t zz = 0; zz < numDims-1 && numVertices+12 < maxVertices; ++zz)
        {
            float rgb[6];
            rgb[2] = zz*invDim;
            rgb[5] = (zz+1)*invDim;

            for (uint32_t yy = 0; yy < numDims-1 && numVertices+12 < maxVertices; ++yy)
            {
                uint32_t offset = (zz*kMaxDims+yy)*kMaxDims;

                rgb[1] = yy*invDim;
                rgb[4] = (yy+1)*invDim;

                for (uint32_t xx = 0; xx < numDims-1 && numVertices+12 < maxVertices; ++xx)
                {
                    uint32_t xoffset = offset + xx;

                    rgb[0] = xx*invDim;
                    rgb[3] = (xx+1)*invDim;

                    float pos[3] =
                    {
                        -kMaxDimsF*0.5f + float(xx)*scale,
                        -kMaxDimsF*0.5f + float(yy)*scale,
                        -kMaxDimsF*0.5f + float(zz)*scale,
                    };

                    const Grid* grid = grid_;
                    const Grid* val[8] = {
                        &grid[xoffset+zpitch+ypitch  ],
                        &grid[xoffset+zpitch+ypitch+1],
                        &grid[xoffset+ypitch+1       ],
                        &grid[xoffset+ypitch         ],
                        &grid[xoffset+zpitch         ],
                        &grid[xoffset+zpitch+1       ],
                        &grid[xoffset+1              ],
                        &grid[xoffset                ],
                    };

                    uint32_t num = triangulate(
                            (uint8_t*)vertex
                        , PosNormalColorVertex::ms_layout.getStride()
                        , rgb
                        , pos
                        , val
                        , iso
                        , scale
                        );
                    vertex += num;
                    numVertices += num;
                }
            }
        }

        profTriangulate = bx::getHPCounter() - profTriangulate;

        float mtx[16];
        bx::mtxRotateXY(mtx, time*0.67f, time);

        // Set model matrix for rendering.
        bgfx::setTransform(mtx);

        // Set vertex and index buffer.
        bgfx::setVertexBuffer(0, &tvb, 0, numVertices);

        // Set render states.
        bgfx::setState(BGFX_STATE_DEFAULT);

        // Submit primitive for rendering to view 0.
        bgfx::submit(viewId_, program_);

        // Display stats.
        ImGui::SetNextWindowPos(
                ImVec2(width() - width() / 5.0f - 10.0f, height() / 7.0f + 30.0f)
            , ImGuiCond_FirstUseEver
            );
        ImGui::SetNextWindowSize(
                ImVec2(width() / 5.0f, height() / 4.0f)
            , ImGuiCond_FirstUseEver
            );
        ImGui::Begin("Stats"
            , NULL
            , 0
            );

        ImGui::Text("Num vertices:"); ImGui::SameLine(100); ImGui::Text("%5d (%6.4f%%)", numVertices, float(numVertices)/maxVertices * 100);
        ImGui::Text("Update:");       ImGui::SameLine(100); ImGui::Text("% 7.3f[ms]", double(profUpdate)*toMs);
        ImGui::Text("Calc normals:"); ImGui::SameLine(100); ImGui::Text("% 7.3f[ms]", double(profNormal)*toMs);
        ImGui::Text("Triangulate:");  ImGui::SameLine(100); ImGui::Text("% 7.3f[ms]", double(profTriangulate)*toMs);
        ImGui::Text("Frame:");        ImGui::SameLine(100); ImGui::Text("% 7.3f[ms]", double(frameTime)*toMs);

        ImGui::End();

    }
    //Data members
	bgfx::ProgramHandle program_;

	Grid* grid_;
	int64_t timeOffset_;
};

EXAMPLE_MAIN(
    ExampleMetaballs
    , "02-metaball"
    , "Rendering with transient buffers and embedding shaders."
    , "https://bkaradzic.github.io/bgfx/examples.html#metaballs"
);
