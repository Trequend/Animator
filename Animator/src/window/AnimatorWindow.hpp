#pragma once

#include "Window.hpp"
#include "../scene/Scene.hpp"

class AnimatorWindow : public Window
{
public:
	AnimatorWindow() : Window("Animator") { }

	void Draw() override;
};
