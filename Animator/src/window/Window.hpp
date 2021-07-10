#pragma once

#include <string>
#include <imgui/imgui.h>

class Window
{
private:
	std::string name;
public:
	Window(const std::string& name, bool isOpened = true)
		: name(name), IsOpened(isOpened) { }

	bool IsOpened;

	const std::string& GetName() const { return name; }

	virtual void Draw() = 0;
};

