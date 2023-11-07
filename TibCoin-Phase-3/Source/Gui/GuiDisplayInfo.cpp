#include "GuiDisplayInfo.h"
#include "Gui.h"

GuiInfo::GuiInfo(const void* const gui): gui(gui) {}

void GuiInfo::setMsg(const std::string& msg) 
{ 
	((Gui*)gui)->updateComMsg(msg);
}