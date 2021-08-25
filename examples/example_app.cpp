#include <bgfx/utils/entry.h>

#include "example_app.h"

namespace bigsaddle {

ExampleApp::ExampleApp(ExampleParams params) : App(params),
    description_(params.description), url_(params.url) {
}

void ExampleApp::Create() {
    App::Create();
    entry::init();
}

} //namespace bigsaddle