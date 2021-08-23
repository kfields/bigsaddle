#include <bx/bx.h>
#include <bx/timer.h>
#include <imgui/imgui.h>
#include <nanovg/nanovg.h>
#include <bigsaddle/app.h>

#define ICON_SEARCH 0x1F50D
#define ICON_CIRCLED_CROSS 0x2716
#define ICON_CHEVRON_RIGHT 0xE75E
#define ICON_CHECK 0x2713
#define ICON_LOGIN 0xE740
#define ICON_TRASH 0xE729

extern char* cpToUTF8(int cp, char* str);
extern void drawWindow(struct NVGcontext* vg, const char* title, float x, float y, float w, float h);
extern void drawSearchBox(struct NVGcontext* vg, const char* text, float x, float y, float w, float h);
extern void drawGraph(struct NVGcontext* vg, float x, float y, float w, float h, float t);
