#pragma once

#include "TypedAnimationClip.hpp"

class ShininessAnimationClip : public TypedAnimationClip<float>
{
protected:
	float Validate(const float& value) override;
public:
	ShininessAnimationClip(const std::string& propertyName, float defaultValue)
		: TypedAnimationClip<float>(propertyName, defaultValue) { }

	ShininessAnimationClip() { }

	void DrawControl(const char* name, float* state) override;
	void DrawPreview(float t) override;
};
