#pragma once

#include <memory>
#include <string>
#include <functional>
#include <unordered_map>
#include <type_traits>
#include "../ConfiguredCereal.hpp"

#include "../Debug.hpp"
#include "AnimationClip.hpp"
#include "TypedAnimationClip.hpp"

class Animator
{
public:
	enum class Mode
	{
		Once,
		Loop,
		PingPong
	};

	static const char* MODE_NAMES[];
	static const int MODES_COUNT;
private:
	class Property
	{
	private:
		std::string name;
	public:
		Property(const std::string& name) : name(name) { }

		const std::string& GetName() const { return name; }

		virtual void ActivateAnimation() = 0;
		virtual void ActivateAnimation(AnimationClip& clip) = 0;
		virtual void DeactivateAnimation() = 0;
		virtual bool AnimationActivated() const = 0;
		virtual std::shared_ptr<AnimationClip> GetAnimation() = 0;
		virtual void Update(float time) = 0;
	};

	template<typename T, typename TClip>
	class TypedProperty : public Property
	{
		static_assert(
			std::is_base_of<TypedAnimationClip<T>, TClip>::value,
			"TClip must inherit from TypedAnimationClip<T>"
		);

	private:
		T* field = nullptr;
		std::function<void(T value)> setter;
		std::shared_ptr<TClip> clip;
		T defaultValue;
	public:
		TypedProperty(const std::string& name, T* field, T defaultValue)
			: Property(name), field(field), defaultValue(defaultValue) { }

		TypedProperty(const std::string& name, std::function<void(T value)> setter, T defaultValue)
			: Property(name), setter(setter), defaultValue(defaultValue) { }

		void ActivateAnimation() override;
		void ActivateAnimation(AnimationClip& clip) override;
		void DeactivateAnimation() override;
		bool AnimationActivated() const override;
		std::shared_ptr<AnimationClip> GetAnimation() override;
		void Update(float time) override;
	};

	std::vector<std::shared_ptr<Property>> properties;
	std::vector<std::shared_ptr<Property>> animatedProperties;
	float animationTime;
	float currentTime = 0.0f;
	Mode mode;
	float timeScale = 1.0f;
public:
	bool Enabled = false;
	bool Paused = false;

	template<typename T, typename TClip>
	void RegisterProperty(const std::string name, T* field, T defaultValue = T());

	template<typename T, typename TClip>
	void RegisterProperty(
		const std::string name,
		std::function<void(T value)> setter,
		T defaultValue = T()
	);

	void ActivatePropertyAnimation(const std::string& name);
	void DeactivatePropertyAnimation(const std::string& name);

	size_t GetAnimatedPropertiesCount() { return animatedProperties.size(); };
	size_t GetPropertiesCount() { return properties.size(); }

	float GetAnimationTime() const { return animationTime; }
	void SetAnimationTime(float value);

	float GetCurrentTime() const { return currentTime; }
	void SetCurrentTime(float value);

	void Update(float deltaTime);

	Mode GetAnimationMode() const { return mode; }
	void SetAnimationMode(Animator::Mode mode);

	const std::string& GetPropertyName(int index) { return properties[index]->GetName(); }

	bool IsAnimatedProperty(int index) { return properties[index]->AnimationActivated(); }

	bool IsDriven(const std::string& propertyName) const;

	std::shared_ptr<AnimationClip> GetPropertyAnimation(size_t index);

	template<class Archive>
	void Save(Archive& archive) const;

	template<class Archive>
	void Load(Archive& archive);
};

template<typename T, typename TClip>
void Animator::TypedProperty<T, TClip>::ActivateAnimation()
{
	DeactivateAnimation();
	clip = std::shared_ptr<TClip>(new TClip(GetName(), defaultValue));
}

template<typename T, typename TClip>
void Animator::TypedProperty<T, TClip>::ActivateAnimation(AnimationClip& clip)
{
	TClip& typedClip = dynamic_cast<TClip&>(clip);
	DeactivateAnimation();
	this->clip = std::shared_ptr<TClip>(new TClip(typedClip));
}

template<typename T, typename TClip>
void Animator::TypedProperty<T, TClip>::DeactivateAnimation()
{
	clip = nullptr;
}

template<typename T, typename TClip>
bool Animator::TypedProperty<T, TClip>::AnimationActivated() const
{
	return clip != nullptr;
}

template<typename T, typename TClip>
void Animator::RegisterProperty(const std::string name, T* field, T defaultValue)
{
	std::shared_ptr<Property> animatorProperty;
	animatorProperty = std::shared_ptr<Property>(new TypedProperty<T, TClip>(name, field, defaultValue));
	properties.push_back(animatorProperty);
}

template<typename T, typename TClip>
void Animator::RegisterProperty(
	const std::string name,
	std::function<void(T value)> setter,
	T defaultValue
)
{
	std::shared_ptr<Property> animatorProperty;
	animatorProperty = std::shared_ptr<Property>(new TypedProperty<T, TClip>(name, setter, defaultValue));
	properties.push_back(animatorProperty);
}

template<typename T, typename TClip>
std::shared_ptr<AnimationClip> Animator::TypedProperty<T, TClip>::GetAnimation()
{
	return clip;
}

template<typename T, typename TClip>
void Animator::TypedProperty<T, TClip>::Update(float time)
{
	T value = clip->Evaluate(time);
	if (field == nullptr)
	{
		setter(value);
	}
	else
	{
		*field = value;
	}
}

template<class Archive>
void Animator::Save(Archive& archive) const
{
	archive(CEREAL_NVP(Enabled));
	archive(CEREAL_NVP(Paused));
	archive(CEREAL_NVP_("AnimationTime", animationTime));
	archive(CEREAL_NVP_("CurrentTime", currentTime));
	archive(CEREAL_NVP_("AnimationMode", mode));
	archive(CEREAL_NVP_("TimeScale", timeScale));
	std::vector<std::shared_ptr<AnimationClip>> clips;
	for (size_t i = 0; i < animatedProperties.size(); i++)
	{
		clips.push_back(animatedProperties[i]->GetAnimation());
	}

	archive(CEREAL_NVP_("AnimationClips", clips));
}

template<class Archive>
void Animator::Load(Archive& archive)
{
	archive(CEREAL_NVP(Enabled));
	archive(CEREAL_NVP(Paused));
	archive(CEREAL_NVP_("AnimationTime", animationTime));
	archive(CEREAL_NVP_("CurrentTime", currentTime));
	archive(CEREAL_NVP_("AnimationMode", mode));
	archive(CEREAL_NVP_("TimeScale", timeScale));
	std::vector<std::shared_ptr<AnimationClip>> clips;
	archive(CEREAL_NVP_("AnimationClips", clips));
	if (animationTime < 0.0f)
	{
		animationTime = 0.0f;
	}

	if (currentTime < 0.0f)
	{
		currentTime = 0.0f;
	}

	if (currentTime > animationTime)
	{
		currentTime = animationTime;
	}

	if (timeScale >= 0.0f)
	{
		timeScale = 1.0f;
	}
	else
	{
		timeScale = -1.0f;
	}

	for (size_t i = 0; i < clips.size(); i++)
	{
		std::shared_ptr<AnimationClip> clip = clips[i];
		bool used = false;
		for (size_t j = 0; j < properties.size(); j++)
		{
			std::shared_ptr<Property> animatorProperty = properties[j];
			if (animatorProperty->GetName() == clip->GetPropertyName())
			{
				if (animatorProperty->AnimationActivated())
				{
					Debug::LogFormat(
						Debug::MessageType::Warning,
						"Animation for property \"%s\" already exists",
						animatorProperty->GetName().c_str()
					);
					break;
				}

				try
				{
					used = true;
					animatorProperty->ActivateAnimation(*clip);
					animatedProperties.push_back(animatorProperty);
				}
				catch (const std::bad_cast&)
				{
					Debug::LogFormat(
						Debug::MessageType::Warning,
						"Wrong type for property animation - \"%s\". The animation will be deleted when the scene is saved",
						animatorProperty->GetName().c_str()
					);
				}

				break;
			}
		}

		if (!used)
		{
			Debug::LogFormat(
				Debug::MessageType::Warning,
				"No property with name - \"%s\". The animation will be deleted when the scene is saved",
				clip->GetPropertyName().c_str()
			);
		}
	}
}
