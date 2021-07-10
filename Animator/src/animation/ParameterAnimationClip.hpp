#pragma once

#include "IntAnimationClip.hpp"

class ParameterAnimationClip : public IntAnimationClip
{
protected:
	int Validate(const int& value) override;
public:
	ParameterAnimationClip(const std::string& propertyName, int defaultValue)
		: IntAnimationClip(propertyName, 3) { }

	ParameterAnimationClip() { }
};
