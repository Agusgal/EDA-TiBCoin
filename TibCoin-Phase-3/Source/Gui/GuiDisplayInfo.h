#pragma once
#include <string>

class GuiInfo
{
public:
	GuiInfo(const void* const gui);

	void setMsg(const std::string& msg);

private:
	const void* const gui;
};