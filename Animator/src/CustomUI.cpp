#include "CustomUI.hpp"

#include "imgui/imgui_internal.h"

#include "scene/Scene.hpp"
#include "Debug.hpp"

bool isDisabledRegion = false;
bool isWasDisabled = false;
std::string reason;

void CustomUI::TextWithEllipsis(const char* text, const char* textEnd, float clipWidth)
{
	float lineWidth = 0.0f;

	if (textEnd == nullptr)
	{
		textEnd = text + strlen(text);
	}

	if (clipWidth == 0.0f)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		clipWidth = ImGui::CalcWrapWidthForPos(window->DC.CursorPos, 0.0f);
	}

	ImFont* font = ImGui::GetFont();
	const char* ellipsis = "...";
	float ellipsisWidth = font->CalcTextSizeA(font->FontSize, FLT_MAX, -1.0, ellipsis).x;

	const char* positionForEllisis = nullptr;

	const char* s = text;
	while (s < textEnd)
	{
		unsigned int c = (unsigned int)*s;
		if (c == 0 || c == '\n')
		{
			positionForEllisis = nullptr;
			break;
		}

		const char* nextS;
		if (c < 0x80)
		{
			nextS = s + 1;
		}
		else
		{
			nextS = s + ImTextCharFromUtf8(&c, s, textEnd);
		}

		if (c == '\r')
		{
			s = nextS;
			continue;
		}

		const float charWidth = (int)c < font->IndexAdvanceX.Size
			? font->IndexAdvanceX.Data[(int)c]
			: font->FallbackAdvanceX;

		if (lineWidth + ellipsisWidth <= clipWidth)
		{
			positionForEllisis = s;
		}

		if (lineWidth + charWidth > clipWidth)
		{
			break;
		}

		lineWidth += charWidth;
		s = nextS;
	}

	if (s == textEnd || positionForEllisis == nullptr)
	{
		ImGui::TextUnformatted(text, s);
	}
	else
	{
		ImGui::TextUnformatted(text, positionForEllisis);
		ImGui::SameLine(0.0f, 0.0f);
		ImGui::TextUnformatted(ellipsis);
	}
}

void CustomUI::BeginDisabledRegion(bool isDisabled, const std::string& reason)
{
	if (isDisabledRegion)
	{
		return;
	}

	::reason = reason;
	isDisabledRegion = true;
	isWasDisabled = isDisabled;
	
	if (isDisabled)
	{
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}
}

void CustomUI::EndDisabledRegion()
{
	if (!isDisabledRegion)
	{
		return;
	}

	isDisabledRegion = false;
	if (isWasDisabled)
	{
		isWasDisabled = false;
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
		{
			ImGui::BeginTooltip();
			ImGui::TextUnformatted(reason.c_str());
			ImGui::EndTooltip();
		}
	}
}

void CustomUI::BeginDrivenRegion(const std::string propertyName)
{
	Scene& scene = Scene::Current();
	BeginDisabledRegion(scene.GetAnimator().IsDriven(propertyName), "Driven by animator");
}

void CustomUI::EndDrivenRegion()
{
	EndDisabledRegion();
}
