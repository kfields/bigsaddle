#pragma once

#include <bigsaddle/app.h>

namespace bigsaddle {

struct ExampleParams : WindowParams {
    ExampleParams(std::string _name = "BigSaddle", std::string _description = "An example",
        std::string _url = "https://bkaradzic.github.io/bgfx/index.html",
        Point _origin = Point(SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED),
        Size _size = Size(800, 600),
        uint32_t _flags = SDL_WINDOW_RESIZABLE
    ) : WindowParams(_name, _origin, _size, _flags) {
        description = _description;
        url = _url;
    }
    std::string description;
    std::string url;
};

class ExampleApp : public App {
public:
    ExampleApp(ExampleParams params = ExampleParams());
    virtual void Create();
    void ShowExampleDialog();

    // Accessors
	const char* getName() const {
		return name_.c_str();
	}
	const char* getDescription() const {
		return description_.c_str();
	}
	const char* getUrl() const {
		return url_.c_str();
	}

    //Data members
    std::string description_;
    std::string url_;
};

} //namespace bigsaddle

#define EXAMPLE_MAIN(_app, ...) \
int main(int argc, char** argv) { \
    _app& app = *new _app(ExampleParams(__VA_ARGS__)); \
    return app.Run(); \
}

#define EXAMPLE_CTOR(_app) \
_app(ExampleParams params = ExampleParams()) \
    : ExampleApp(params) { \
}

/*
	int _main_(int _argc, char** _argv)                 \
	{                                                   \
			_app app(__VA_ARGS__);                      \
			return entry::runApp(&app, _argc, _argv);   \
	}
*/