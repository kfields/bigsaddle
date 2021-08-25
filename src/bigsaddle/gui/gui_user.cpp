#include <imgui/imgui.h>

#include "gui_font.h"
#include "gui_user.h"

namespace ImGui {

void PushFont(GuiFont::Kind kind)
{
	::ImGui::PushFont(GuiFonts::instance().fonts_[kind]);
}

} //namespace ImGui