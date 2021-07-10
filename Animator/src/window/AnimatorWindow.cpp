#include "AnimatorWindow.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui_internal.h>

#include "../scene/Scene.hpp"
#include "../CustomUI.hpp"

void AnimatorWindow::Draw()
{
	const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_Framed
		| ImGuiTreeNodeFlags_SpanAvailWidth
		| ImGuiTreeNodeFlags_AllowItemOverlap
		| ImGuiTreeNodeFlags_FramePadding;

	Scene& scene = Scene::Current();
	Animator& animator = scene.GetAnimator();

	ImGui::Begin(GetName().c_str(), nullptr, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar())
	{
		if (animator.Enabled)
		{
			if (ImGui::MenuItem("Disable"))
			{
				animator.Enabled = false;
			}
		}
		else
		{
			if (ImGui::MenuItem("Enable"))
			{
				animator.Enabled = true;
			}
		}

		if (animator.Paused)
		{
			if (
				animator.GetAnimationMode() == Animator::Mode::Once
				&& animator.GetCurrentTime() >= animator.GetAnimationTime()
			)
			{
				if (ImGui::MenuItem("Restart"))
				{
					animator.SetCurrentTime(0.0f);
					animator.Paused = false;
				}
			}
			else
			{
				if (ImGui::MenuItem("Resume"))
				{
					animator.Paused = false;
				}
			}
		}
		else
		{
			if (ImGui::MenuItem("Pause"))
			{
				animator.Paused = true;
			}
		}

		CustomUI::BeginDisabledRegion(
			animator.GetPropertiesCount() == animator.GetAnimatedPropertiesCount(),
			"No properties"
		);
		if (ImGui::BeginMenu("Add property"))
		{
			for (size_t i = 0; i < animator.GetPropertiesCount(); i++)
			{
				if (!animator.IsAnimatedProperty(i))
				{
					const std::string& name = animator.GetPropertyName(i);
					if (ImGui::MenuItem(name.c_str()))
					{
						animator.ActivatePropertyAnimation(name);
					}
				}
			}
			ImGui::EndMenu();
		}
		CustomUI::EndDisabledRegion();

		ImGui::EndMenuBar();
	}

	float animationTime = animator.GetAnimationTime();
	if (ImGui::DragFloat("Animation time", &animationTime))
	{
		animator.SetAnimationTime(animationTime);
	}

	float currentTime = animator.GetCurrentTime();
	if (ImGui::SliderFloat("Time", &currentTime, 0.0f, animator.GetAnimationTime()))
	{
		animator.Paused = true;
		animator.SetCurrentTime(currentTime);
	}

	int modeIndex = (int)animator.GetAnimationMode();
	if (ImGui::Combo("Mode", &modeIndex, Animator::MODE_NAMES, Animator::MODES_COUNT))
	{
		animator.SetAnimationMode((Animator::Mode)modeIndex);
	}

	char buffer[20];
	for (size_t i = 0; i < animator.GetAnimatedPropertiesCount(); i++)
	{
		std::shared_ptr<AnimationClip> clip = animator.GetPropertyAnimation(i);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		bool open = ImGui::TreeNodeEx(clip->GetPropertyName().c_str(), treeNodeFlags);
		ImGui::PopStyleVar();
		ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
		sprintf_s(buffer, "x##%d", i);
		if (ImGui::Button(buffer, ImVec2{ lineHeight, lineHeight }))
		{
			animator.DeactivatePropertyAnimation(clip->GetPropertyName());
			i--;
		}
		if (open)
		{
			clip->DrawUI();
			ImGui::TreePop();
		}
	}
	ImGui::End();
}
