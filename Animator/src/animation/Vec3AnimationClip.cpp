#include "Vec3AnimationClip.hpp"

#include <imgui/imgui.h>

void Vec3AnimationClip::DrawControl(const char* name, glm::vec3* state)
{
	ImGui::DragFloat3(name, &(*state)[0]);
}

void Vec3AnimationClip::DrawPreview(float t)
{
	glm::vec3 value = EvaluateParameter(t);
	ImGui::Text("Value - (%0.3f, %0.3f, %0.3f)", value.x, value.y, value.z);

	float x[PLOT_LINES_FREQUENCY + 1];
	float y[PLOT_LINES_FREQUENCY + 1];
	float z[PLOT_LINES_FREQUENCY + 1];
	float step = 1.0f / PLOT_LINES_FREQUENCY;
	for (size_t i = 0; i <= PLOT_LINES_FREQUENCY; i++)
	{
		value = EvaluateParameter(step * i);
		x[i] = value.x;
		y[i] = value.y;
		z[i] = value.z;
	}

	ImGui::PlotLines("X", x, PLOT_LINES_FREQUENCY + 1);
	ImGui::PlotLines("Y", y, PLOT_LINES_FREQUENCY + 1);
	ImGui::PlotLines("Z", z, PLOT_LINES_FREQUENCY + 1);
}
