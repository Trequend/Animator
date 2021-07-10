#include "DebugWindow.hpp"

#include <memory>

#include "../CustomUI.hpp"

void DebugWindow::DrawIcon(std::shared_ptr<Texture> texture)
{
	ImGui::Image(
		(void*)texture->GetId(),
		ImVec2{ 36.0f, 36.0f },
		ImVec2{ 1.0f, 1.0f },
		ImVec2{ 0.0f, 0.0f }
	);
}

void DebugWindow::Splitter()
{
	using namespace ImGui;
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	ImGuiID id = window->GetID("##Splitter");
	ImRect bb;
	ImVec2 vMin = ImGui::GetWindowContentRegionMin();
	ImVec2 vMax = ImGui::GetWindowContentRegionMax();
	float height = vMax.y - vMin.y;
	float size1 = height * listPercent;
	float size2 = height * messagePercent;
	bb.Min = window->DC.CursorPos;
	bb.Min.y += size1;
	ImVec2 itemSize = CalcItemSize(ImVec2(-1.0f, 2.0f), 0.0f, 0.0f);
	bb.Max = bb.Min;
	bb.Max.x += itemSize.x;
	bb.Max.y += itemSize.y;
	SplitterBehavior(bb, id, ImGuiAxis_Y, &size1, &size2, 10.0f, 30.0f, 0.0f);
	listPercent = size1 / height;
	messagePercent = size2 / height;
}

void DebugWindow::ListPart(float height)
{
	ImGui::BeginChild("MessageList", ImVec2(-1, height * listPercent), false);
	char buffer[100];
	ImVec2 contentRegion = ImGui::GetWindowContentRegionMax();
	int lastElement = -1;
	ImGuiListClipper clipper(Debug::GetMessagesCount(), 55.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.0f, 0.0f });
	while (clipper.Step())
	{
		for (int n = clipper.DisplayStart; n < clipper.DisplayEnd; n++)
		{
			sprintf_s(buffer, "##%d", n);
			if (ImGui::Selectable(buffer, selected == n, 0, ImVec2(0, 55.0f)))
			{
				selected = selected == n ? -1 : n;
			}

			ImGui::SameLine();
			ImGui::BeginGroup();
			ImGui::InvisibleButton("##vertical padding", ImVec2{ -1.0f, 11.5f });
			if (contentRegion.x < 50.0f)
			{
				ImGui::EndGroup();
				continue;
			}

			ImGui::InvisibleButton("##horizontal padding", ImVec2{ 10.0f, 43.5f });
			ImGui::SameLine();
			const Debug::Message& message = Debug::GetMessage(n);
			switch (message.Type)
			{
			case Debug::MessageType::Info:
				DrawIcon(info);
				break;
			case Debug::MessageType::Warning:
				DrawIcon(warning);
				break;
			case Debug::MessageType::Error:
				DrawIcon(error);
				break;
			}

			if (contentRegion.x < 75.0f)
			{
				ImGui::EndGroup();
				continue;
			}

			ImGui::SameLine(0.0f, 10.0f);
			CustomUI::TextWithEllipsis(message.Time);

			if (contentRegion.x < 160.0f)
			{
				ImGui::EndGroup();
				continue;
			}

			ImGui::SameLine(0.0f, 5.0f);
			CustomUI::TextWithEllipsis(message.Content, message.Content + message.ContentLength);

			ImGui::EndGroup();
		}
	}
	ImGui::PopStyleVar(1);
	clipper.End();
	if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
	{
		ImGui::SetScrollHereY();
	}

	ImGui::EndChild();
}

void DebugWindow::MessagePart(float height)
{
	ImGui::BeginChild("Message", ImVec2(-1, messagePercent * height - 5.0f), false);
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
	ImGui::PushTextWrapPos(ImGui::GetWindowContentRegionMax().x - 5.0f);
	if (selected != -1)
	{
		const Debug::Message& message = Debug::GetMessage(selected);
		ImGui::TextWrapped(message.Content);
	}
	ImGui::PopTextWrapPos();
	ImGui::EndChild();
}

void DebugWindow::Draw()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::MenuItem("Clear"))
		{
			Debug::Clear();
		}

		ImGui::EndMenuBar();
	}

	ImVec2 vMin = ImGui::GetWindowContentRegionMin();
	ImVec2 vMax = ImGui::GetWindowContentRegionMax();
	float height = vMax.y - vMin.y;
	
	Splitter();
	ListPart(height);
	MessagePart(height);

	ImGui::End();
	ImGui::PopStyleVar(ImGuiStyleVar_WindowPadding);
}
