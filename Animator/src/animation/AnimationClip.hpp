#pragma once

#include <vector>
#include <string>
#include "../ConfiguredCereal.hpp"

class AnimationClip
{
private:
	std::string propertyName;
public:
	AnimationClip(const std::string& propertyName)
		: propertyName(propertyName) { }

	AnimationClip() { }

	const std::string& GetPropertyName() const { return propertyName; }

	virtual void DrawUI() = 0;

	template<class Archive>
	void Save(Archive& archive) const;

	template<class Archive>
	void Load(Archive& archive);
};

template<class Archive>
void AnimationClip::Save(Archive& archive) const
{
	archive(CEREAL_NVP_("PropertyName", propertyName));
}

template<class Archive>
void AnimationClip::Load(Archive& archive)
{
	archive(CEREAL_NVP_("PropertyName", propertyName));
}
