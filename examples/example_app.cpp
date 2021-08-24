#include <bgfx/utils/entry.h>

#include "example_app.h"

namespace bigsaddle {

void ExampleApp::Create() {
    App::Create();
    entry::init();
}

} //namespace bigsaddle