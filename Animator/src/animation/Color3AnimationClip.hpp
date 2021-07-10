#pragma once

#include <glm/glm.hpp>

#include "TypedAnimationClip.hpp"

class Color3AnimationClip : public TypedAnimationClip<glm::vec3>
{
public:
	Color3AnimationClip(const std::string& propertyName, glm::vec3 defaultValue)
		: TypedAnimationClip<glm::vec3>(propertyName, defaultValue) { }

	Color3AnimationClip() { }

	void DrawControl(const char* name, glm::vec3* state) override;
	void DrawPreview(float t) override;
};
