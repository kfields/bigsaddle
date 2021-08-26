#include <imgui.h>

#include <bx/math.h>
#include <bx/timer.h>

#include <bgfx/utils/utils.h>

#include <examples/example_app.h>

using namespace bigsaddle;

class ExampleMesh : public ExampleApp {
public:
    EXAMPLE_CTOR(ExampleMesh)

    virtual void Create() override {
        ExampleApp::Create();

        u_time = bgfx::createUniform("u_time", bgfx::UniformType::Vec4);

        // Create program from shaders.
        m_program = loadProgram("vs_mesh", "fs_mesh");

        m_mesh = meshLoad("meshes/bunny.bin");

        m_timeOffset = bx::getHPCounter();
    }

    virtual void Destroy() override {
        meshUnload(m_mesh);

        bgfx::destroy(m_program);
        bgfx::destroy(u_time);

        ExampleApp::Destroy();
    }

    virtual void Draw() override {
        ExampleApp::Draw();
        ShowExampleDialog();

        // Set view 0 default viewport.
        bgfx::setViewRect(0, 0, 0, uint16_t(width()), uint16_t(height()));

        // This dummy draw call is here to make sure that view 0 is cleared
        // if no other draw calls are submitted to view 0.
        bgfx::touch(0);

        float time = (float)((bx::getHPCounter() - m_timeOffset) / double(bx::getHPFrequency()));
        bgfx::setUniform(u_time, &time);

        const bx::Vec3 at = { 0.0f, 1.0f,  0.0f };
        const bx::Vec3 eye = { 0.0f, 1.0f, -2.5f };

        // Set view and projection matrix for view 0.
        {
            float view[16];
            bx::mtxLookAt(view, eye, at);

            float proj[16];
            bx::mtxProj(proj, 60.0f, float(width()) / float(height()), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
            bgfx::setViewTransform(0, view, proj);

            // Set view 0 default viewport.
            bgfx::setViewRect(0, 0, 0, uint16_t(width()), uint16_t(height()));
        }

        float mtx[16];
        bx::mtxRotateXY(mtx
            , 0.0f
            , time * 0.37f
        );

        meshSubmit(m_mesh, 0, m_program, mtx);

    }
    //Data members
    int64_t m_timeOffset;
    Mesh* m_mesh;
    bgfx::ProgramHandle m_program;
    bgfx::UniformHandle u_time;
};

EXAMPLE_MAIN(
    ExampleMesh
    , "04-mesh"
    , "Loading meshes."
    , "https://bkaradzic.github.io/bgfx/examples.html#mesh"
);
