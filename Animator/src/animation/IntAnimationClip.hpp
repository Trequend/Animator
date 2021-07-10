#pragma once

#include "TypedAnimationClip.hpp"

class IntAnimationClip : public TypedAnimationClip<int>
{
public:
	IntAnimationClip(const std::string& propertyName, int defaultValue)
		: TypedAnimationClip<int>(propertyName, defaultValue) { }

	IntAnimationClip() { }

	void DrawControl(const char* name, int* state) override;
	void DrawPreview(float t) override;
};
