#include <string>
#include <iostream>

#include "SDL.h"
#include "SDL_syswm.h"

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/math.h>

#include <imgui.h>

#include <bigsaddle/gui/gui.h>
#include <bigsaddle/app.h>

using namespace bigsaddle;

class MyApp : public App {
public:
    MyApp() {
    }
    virtual void DoDraw() override {
        App::DoDraw();
        ImGui::ShowDemoWindow();

        // simple input code for orbit camera
        int mouse_x, mouse_y;
        const int buttons = SDL_GetGlobalMouseState(&mouse_x, &mouse_y);
        if ((buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0) {
            int delta_x = mouse_x - prev_mouse_x_;
            int delta_y = mouse_y - prev_mouse_y_;
            cam_yaw_ += float(-delta_x) * rot_scale_;
            cam_pitch_ += float(-delta_y) * rot_scale_;
        }

        prev_mouse_x_ = mouse_x;
        prev_mouse_y_ = mouse_y;

        float cam_rotation[16];
        bx::mtxRotateXYZ(cam_rotation, cam_pitch_, cam_yaw_, 0.0f);

        float cam_translation[16];
        bx::mtxTranslate(cam_translation, 0.0f, 0.0f, -5.0f);

        float cam_transform[16];
        bx::mtxMul(cam_transform, cam_translation, cam_rotation);

        float view[16];
        bx::mtxInverse(view, cam_transform);

        float proj[16];
        bx::mtxProj(
            proj, 60.0f, float(width()) / float(height()), 0.1f, 100.0f,
            bgfx::getCaps()->homogeneousDepth);

        bgfx::setViewTransform(0, view, proj);

        float model[16];
        bx::mtxIdentity(model);
        bgfx::setTransform(model);
    }
    //Data members
    float cam_pitch_ = 0.0f;
    float cam_yaw_ = 0.0f;
    float rot_scale_ = 0.01f;

    int prev_mouse_x_ = 0;
    int prev_mouse_y_ = 0;
};

int main(int argc, char** argv)
{
    MyApp app;

    return app.Run();
}
