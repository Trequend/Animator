#pragma once

#include <glm/glm.hpp>

#include "ConfiguredCereal.hpp"

#include "animation/AnimationClip.hpp"
#include "animation/Vec3AnimationClip.hpp"
#include "animation/IntAnimationClip.hpp"
#include "animation/ParameterAnimationClip.hpp"
#include "animation/Color3AnimationClip.hpp"
#include "animation/Color4AnimationClip.hpp"

CEREAL_REGISTER_TYPE(Vec3AnimationClip);
CEREAL_REGISTER_TYPE(IntAnimationClip);
CEREAL_REGISTER_TYPE(ParameterAnimationClip);
CEREAL_REGISTER_TYPE(Color3AnimationClip);
CEREAL_REGISTER_TYPE(Color4AnimationClip);

CEREAL_REGISTER_POLYMORPHIC_RELATION(AnimationClip, Vec3AnimationClip);
CEREAL_REGISTER_POLYMORPHIC_RELATION(AnimationClip, ParameterAnimationClip);
CEREAL_REGISTER_POLYMORPHIC_RELATION(AnimationClip, IntAnimationClip);
CEREAL_REGISTER_POLYMORPHIC_RELATION(AnimationClip, Color3AnimationClip);
CEREAL_REGISTER_POLYMORPHIC_RELATION(AnimationClip, Color4AnimationClip);

namespace cereal
{
	template<class Archive>
	void Serialize(Archive& archive, glm::vec3& vec)
	{
		archive(cereal::make_nvp("X", vec.x));
		archive(cereal::make_nvp("Y", vec.y));
		archive(cereal::make_nvp("Z", vec.z));
	}

	template<class Archive>
	void Serialize(Archive& archive, glm::vec4& vec)
	{
		archive(cereal::make_nvp("X", vec.x));
		archive(cereal::make_nvp("Y", vec.y));
		archive(cereal::make_nvp("Z", vec.z));
		archive(cereal::make_nvp("W", vec.w));
	}
}
