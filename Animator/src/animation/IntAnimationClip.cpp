#include "IntAnimationClip.hpp"

#include <cmath>

void IntAnimationClip::DrawControl(const char* name, int* state)
{
	ImGui::InputInt(name, state);
}

void IntAnimationClip::DrawPreview(float t)
{
	int value = EvaluateParameter(t);
	ImGui::Text("Value - %d", value);

	float values[PLOT_LINES_FREQUENCY + 1];
	float step = 1.0f / PLOT_LINES_FREQUENCY;
	for (size_t i = 0; i <= PLOT_LINES_FREQUENCY; i++)
	{
		values[i] = (float)EvaluateParameter(step * i);
	}

	ImGui::PlotLines("V", values, PLOT_LINES_FREQUENCY + 1);
}
