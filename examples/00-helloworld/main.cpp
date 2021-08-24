#include <imgui.h>

#include <examples/example_app.h>

using namespace bigsaddle;

class MyApp : public ExampleApp {
public:
    MyApp() {
    }
    virtual void Draw() override {
        ExampleApp::Draw();
        ImGui::ShowDemoWindow();
    }
    //Data members
};

int main(int argc, char** argv)
{
    MyApp& app = *new MyApp();

    return app.Run();
}
