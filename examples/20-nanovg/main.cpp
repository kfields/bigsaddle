#include "main.h"

#include <examples/example_app.h>

using namespace bigsaddle;

// void renderDemo(struct NVGcontext* vg, float mx, float my, float width, float height, float t, int blowup, struct DemoData* data)
// void renderDemo(struct NVGcontext* vg, float width, float height, float t)
void renderDemo(struct NVGcontext *vg, float mx, float my, float width, float height, float t)
{
	float x, y, popx, popy;

	drawEyes(vg, width - 800, height - 240, 150, 100, mx, my, t);
	drawParagraph(vg, width - 550, 35, 150, 100, mx, my);
	drawGraph(vg, 0, height / 2, width, height / 2, t);

	drawColorwheel(vg, width - 350, 35, 250.0f, 250.0f, t);

	// Line joints
	drawLines(vg, 50, height - 50, 600, 35, t);

	// Line caps
	drawWidths(vg, width - 50, 35, 30);

	// Line caps
	drawCaps(vg, width - 50, 260, 30);

	drawScissor(vg, 40, height - 150, t);

	nvgSave(vg);
	/*if (blowup)
	{
		nvgRotate(vg, sinf(t * 0.3f) * 5.0f / 180.0f * NVG_PI);
		nvgScale(vg, 2.0f, 2.0f);
	}*/

	// Widgets.
	x = width - 520;
	y = height - 420;
	drawWindow(vg, "Widgets `n Stuff", x, y, 300, 400);
	x += 10;
	y += 45;
	drawSearchBox(vg, "Search", x, y, 280, 25);
	y += 40;
	/*drawDropDown(vg, "Effects", x, y, 280, 28);
	popx = x + 300;
	popy = y + 14;
	y += 45;

	// Form
	drawLabel(vg, "Login", x, y, 280, 20);
	y += 25;
	drawEditBox(vg, "Email", x, y, 280, 28);
	y += 35;
	drawEditBox(vg, "Password", x, y, 280, 28);
	y += 38;
	drawCheckBox(vg, "Remember me", x, y, 140, 28);
	drawButton(vg, ICON_LOGIN, "Sign in", x + 138, y, 140, 28, nvgRGBA(0, 96, 128, 255));
	y += 45;

	// Slider
	drawLabel(vg, "Diameter", x, y, 280, 20);
	y += 25;
	drawEditBoxNum(vg, "123.00", "px", x + 180, y, 100, 28);
	drawSlider(vg, 0.4f, x, y, 170, 28);
	y += 55;

	drawButton(vg, ICON_TRASH, "Delete", x, y, 160, 28, nvgRGBA(128, 16, 8, 255));
	drawButton(vg, 0, "Cancel", x + 170, y, 110, 28, nvgRGBA(0, 0, 0, 0));

	// Thumbnails box
	drawThumbnails(vg, popx, popy - 30, 160, 300, data->images, 12, t);

	// Blendish
	drawBlendish(vg, 10, 62, 600.0f, 420.0f, t);
	*/
	nvgRestore(vg);
}

class ExampleNanoVG : public ExampleApp
{
public:
	ExampleNanoVG(ExampleParams params) : ExampleApp(params),
																				nvg_(nullptr)
	{
	}
	virtual void Create() override
	{
		ExampleApp::Create();
		timeOffset_ = bx::getHPCounter();
		int32_t edgeAntiAlias = 1;
		bgfx::ViewId viewId = AllocViewId();
		bgfx::setViewName(viewId, "NanoVG");
		bgfx::setViewMode(viewId, bgfx::ViewMode::Sequential);
		nvg_ = nvgCreate(edgeAntiAlias, viewId);
		loadDemoData(nvg_, &data_);
	}
	virtual void Destroy() override
	{
		freeDemoData(nvg_, &data_);
		nvgDelete(nvg_);
		ExampleApp::Destroy();
	}
	virtual void Draw() override
	{
		ExampleApp::Draw();
		ShowExampleDialog();

		float width = size_.width;
		float height = size_.height;
		float x, y, popx, popy;
		x = width - 520;
		y = height - 420;

		nvgBeginFrame(nvg_, width, height, 1.0f);

		int64_t now = bx::getHPCounter();
		const double freq = double(bx::getHPFrequency());
		float time = (float)((now - timeOffset_) / freq);

		float mx, my;
		Uint32 mState = SDL_GetMouseState(&mx, &my);
		// renderDemo(nvg_, float(m_mouseState.m_mx), float(m_mouseState.m_my), float(m_width), float(m_height), time, 0, &m_data);
		renderDemo(nvg_, mx, my, width, height, time);
		nvgEndFrame(nvg_);
	}
	// Data members
	NVGcontext *nvg_;
	int64_t timeOffset_;
	DemoData data_;
};

EXAMPLE_MAIN(
		ExampleNanoVG, "20-nanovg", "NanoVG is small antialiased vector graphics rendering library.", "https://bkaradzic.github.io/bgfx/examples.html#nanovg");
