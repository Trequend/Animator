#pragma once

#include <memory>

#include "Window.hpp"
#include "../Debug.hpp"
#include "../Texture.hpp"

class DebugWindow : public Window
{
private:
	const std::shared_ptr<Texture> error = Texture::Load("assets/textures/debug_window/error.png");
	const std::shared_ptr<Texture> info = Texture::Load("assets/textures/debug_window/info.png");
	const std::shared_ptr<Texture> warning = Texture::Load("assets/textures/debug_window/warning.png");

	float listPercent = 0.7f;
	float messagePercent = 0.3f;

	int selected = -1;

	void DrawIcon(std::shared_ptr<Texture> texture);

	void Splitter();
	void ListPart(float height);
	void MessagePart(float height);
public:
	DebugWindow() : Window("Debug") { }

	void Draw() override;
};
