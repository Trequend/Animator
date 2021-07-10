#include "ParameterAnimationClip.hpp"

#include <cmath>

int ParameterAnimationClip::Validate(const int& value)
{
	if (value < 3)
	{
		return 3;
	}

	return value;
}
