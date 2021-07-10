#include "Animator.hpp"

const char* Animator::MODE_NAMES[] = { "Once", "Loop", "Ping pong" };
const int Animator::MODES_COUNT = sizeof(Animator::MODE_NAMES) / sizeof(char*);

void Animator::ActivatePropertyAnimation(const std::string& name)
{
	for (size_t i = 0; i < properties.size(); i++)
	{
		if (properties[i]->GetName() == name)
		{
			if (properties[i]->AnimationActivated())
			{
				return;
			}

			properties[i]->ActivateAnimation();
			animatedProperties.push_back(properties[i]);
			break;
		}
	}
}

void Animator::DeactivatePropertyAnimation(const std::string& name)
{
	for (size_t i = 0; i < animatedProperties.size(); i++)
	{
		if (animatedProperties[i]->GetName() == name)
		{
			animatedProperties[i]->DeactivateAnimation();
			animatedProperties.erase(animatedProperties.begin() + i);
			break;
		}
	}
}

std::shared_ptr<AnimationClip> Animator::GetPropertyAnimation(size_t index)
{
	return animatedProperties[index]->GetAnimation();
}

void Animator::SetAnimationMode(Mode mode)
{
	if (this->mode == mode)
	{
		return;
	}

	currentTime = 0.0f;
	this->mode = mode;
	timeScale = 1.0f;
	Paused = false;
}

void Animator::SetAnimationTime(float value)
{
	if (value < 0.0f)
	{
		value = 0.0f;
	}

	animationTime = value;
}

bool Animator::IsDriven(const std::string& propertyName) const
{
	if (!Enabled)
	{
		return false;
	}

	for (size_t i = 0; i < animatedProperties.size(); i++)
	{
		if (animatedProperties[i]->GetName() == propertyName)
		{
			return true;
		}
	}

	return false;
}

void Animator::SetCurrentTime(float value)
{
	if (value < 0.0f)
	{
		value = 0.0f;
	}

	if (value > animationTime)
	{
		value = animationTime;
	}

	currentTime = value;
}

void Animator::Update(float deltaTime)
{
	if (!Enabled)
	{
		return;
	}

	if (!Paused && animationTime > 0.0f)
	{
		currentTime += deltaTime * timeScale;
		switch (mode)
		{
		case Animator::Mode::Once:
			if (currentTime > animationTime)
			{
				currentTime = animationTime;
				Paused = true;
			}
			break;
		case Animator::Mode::Loop:
			if (currentTime > animationTime)
			{
				currentTime = 0.0f;
			}
			break;
		case Animator::Mode::PingPong:
			if (currentTime < 0.0f)
			{
				timeScale *= -1;
				currentTime = 0.0f;
			}
			else if (currentTime > animationTime)
			{
				timeScale *= -1;
				currentTime = animationTime;
			}
			break;
		}
	}

	for (size_t i = 0; i < animatedProperties.size(); i++)
	{
		animatedProperties[i]->Update(currentTime);
	}
}
