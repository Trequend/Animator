#include "Color3AnimationClip.hpp"

#include <imgui/imgui.h>

void Color3AnimationClip::DrawControl(const char* name, glm::vec3* state)
{
	ImGui::ColorEdit3(name, &(*state)[0]);
}

void Color3AnimationClip::DrawPreview(float t)
{
	glm::vec3 value = EvaluateParameter(t);
	ImGui::Text("RGBA: %d, %d, %d", int(value[0] * 255.0f), int(value[1] * 255.0f), int(value[2] * 255.0f));

	float r[PLOT_LINES_FREQUENCY + 1];
	float g[PLOT_LINES_FREQUENCY + 1];
	float b[PLOT_LINES_FREQUENCY + 1];
	float step = 1.0f / PLOT_LINES_FREQUENCY;
	for (size_t i = 0; i <= PLOT_LINES_FREQUENCY; i++)
	{
		value = EvaluateParameter(step * i);
		r[i] = value[0];
		g[i] = value[1];
		b[i] = value[2];
	}

	ImGui::PlotLines("R", r, PLOT_LINES_FREQUENCY + 1);
	ImGui::PlotLines("G", g, PLOT_LINES_FREQUENCY + 1);
	ImGui::PlotLines("B", b, PLOT_LINES_FREQUENCY + 1);
}
