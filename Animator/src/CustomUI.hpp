#pragma once

#include <string>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace CustomUI
{
	void TextWithEllipsis(const char* text, const char* textEnd = 0, float clipWidth = 0.0f);

	void BeginDisabledRegion(bool isDisabled, const std::string& reason);

	void EndDisabledRegion();

	void BeginDrivenRegion(const std::string propertyName);

	void EndDrivenRegion();
}

