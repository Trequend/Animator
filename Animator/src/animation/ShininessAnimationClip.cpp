#include "ShininessAnimationClip.hpp"

float ShininessAnimationClip::Validate(const float& value)
{
	if (value < 0.0f)
	{
		return 0.0f;
	}
	else if (value > 128.0f)
	{
		return 128.0f;
	}

	return value;
}

void ShininessAnimationClip::DrawControl(const char* name, float* state)
{
	ImGui::SliderFloat(name, state, 0.0f, 128.0f);
}

void ShininessAnimationClip::DrawPreview(float t)
{
	float value = EvaluateParameter(t);
	ImGui::Text("Value - %f", value);

	float values[PLOT_LINES_FREQUENCY + 1];
	float step = 1.0f / PLOT_LINES_FREQUENCY;
	for (size_t i = 0; i <= PLOT_LINES_FREQUENCY; i++)
	{
		values[i] = EvaluateParameter(step * i);
	}

	ImGui::PlotLines("V", values, PLOT_LINES_FREQUENCY + 1);
}
