#include <imgui.h>

#include <bigsaddle/app.h>

using namespace bigsaddle;

class MyApp : public App {
public:
    MyApp() {
    }
    virtual void Draw() override {
        App::Draw();
        ImGui::ShowDemoWindow();
    }
    //Data members
};

int main(int argc, char** argv)
{
    MyApp& app = *new MyApp();

    return app.Run();
}
