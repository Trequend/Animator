#pragma once

#include <glm/glm.hpp>

#include "TypedAnimationClip.hpp"

class Color4AnimationClip : public TypedAnimationClip<glm::vec4>
{
public:
	Color4AnimationClip(const std::string& propertyName, glm::vec4 defaultValue)
		: TypedAnimationClip<glm::vec4>(propertyName, defaultValue) { }

	Color4AnimationClip() { }

	void DrawControl(const char* name, glm::vec4* state) override;
	void DrawPreview(float t) override;
};
