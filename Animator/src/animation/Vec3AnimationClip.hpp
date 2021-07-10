#pragma once

#include <glm/glm.hpp>

#include "TypedAnimationClip.hpp"

class Vec3AnimationClip : public TypedAnimationClip<glm::vec3>
{
public:
	Vec3AnimationClip(const std::string& propertyName, glm::vec3 defaultValue)
		: TypedAnimationClip<glm::vec3>(propertyName, defaultValue) { }

	Vec3AnimationClip() { }

	void DrawControl(const char* name, glm::vec3* state) override;
	void DrawPreview(float t) override;
};
