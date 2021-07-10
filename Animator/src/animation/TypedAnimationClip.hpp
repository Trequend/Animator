#pragma once

#include <imgui/imgui.h>
#include "../ConfiguredCereal.hpp"

#include "AnimationClip.hpp"

template<typename T>
class TypedAnimationClip : public AnimationClip
{
private:
	static const int MIN_STATES_COUNT = 2;

	float startTime = 0.0f;
	float endTime = 0.0f;
	std::vector<T> states;
	float previewT = 0.0f;

	T defaultValue;

	void ChangeStatesCount(int value);
protected:
	static const int PLOT_LINES_FREQUENCY = 20;

	virtual void DrawControl(const char* name, T* state) = 0;
	virtual void DrawPreview(float t) = 0;

	virtual T Validate(const T& value) { return value; };

	T EvaluateParameter(float t);
public:
	TypedAnimationClip(const std::string& propertyName, T defaultValue)
		: AnimationClip(propertyName), defaultValue(defaultValue)
	{
		ChangeStatesCount(MIN_STATES_COUNT);
	}

	TypedAnimationClip() { }

	const float GetStartTime() const { return startTime; }
	const float GetEndTime() const { return endTime; }

	T Evaluate(float time);
	void DrawUI() override final;

	template<class Archive>
	void Save(Archive& archive) const;

	template<class Archive>
	void Load(Archive& archive);
};

template<typename T>
void TypedAnimationClip<T>::ChangeStatesCount(int value)
{
	if (value < MIN_STATES_COUNT)
	{
		value = MIN_STATES_COUNT;
	}

	size_t start = states.size();
	states.resize(value);
	for (size_t i = start; i < states.size(); i++)
	{
		states[i] = Validate(defaultValue);
	}
}

template<typename T>
T TypedAnimationClip<T>::EvaluateParameter(float t)
{
	T* points = new T[states.size()];
	for (size_t i = 0; i < states.size(); i++)
	{
		points[i] = states[i];
	}

	for (int limit = states.size(); limit > 1; limit--)
	{
		for (int i = 0; i < limit - 1; i++)
		{
			points[i] = (T)((1 - t) * points[i] + t * points[i + 1]);
		}
	}

	T result = points[0];
	delete[] points;
	return result;
}

template<typename T>
T TypedAnimationClip<T>::Evaluate(float time)
{
	if (time < startTime)
	{
		time = startTime;
	}
	else if (time > endTime)
	{
		time = endTime;
	}

	float timeDelta = endTime - startTime;
	float t = timeDelta == 0.0f ? 0.0f : (time - startTime) / timeDelta;
	return EvaluateParameter(t);
}

template<typename T>
void TypedAnimationClip<T>::DrawUI()
{
	float temp = startTime;
	if (ImGui::DragFloat("Start time", &temp))
	{
		if (temp < 0.0f)
		{
			temp = 0.0f;
		}

		startTime = temp;
	}

	temp = endTime;
	if (ImGui::DragFloat("End time", &temp))
	{
		if (temp < 0.0f)
		{
			temp = 0.0f;
		}

		endTime = temp;
	}

	if (endTime < startTime)
	{
		endTime = startTime;
	}

	int statesCount = states.size();
	if (ImGui::InputInt("States count", &statesCount))
	{
		ChangeStatesCount(statesCount);
	}

	char buffer[30];
	statesCount = states.size();
	for (int i = 0; i < statesCount; i++)
	{
		sprintf_s(buffer, "S%d", i);
		DrawControl(buffer, &states[i]);
		states[i] = Validate(states[i]);
	}

	const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_SpanAvailWidth
		| ImGuiTreeNodeFlags_AllowItemOverlap
		| ImGuiTreeNodeFlags_FramePadding;
	
	bool open = ImGui::TreeNodeEx("Preview", treeNodeFlags);
	if (open)
	{
		ImGui::SliderFloat("T", &previewT, 0.0f, 1.0f);
		DrawPreview(previewT);
		ImGui::TreePop();
	}
}

template<typename T>
template<class Archive>
void TypedAnimationClip<T>::Save(Archive& archive) const
{
	archive(CEREAL_NVP_("BaseInfo", cereal::base_class<AnimationClip>(this)));
	archive(CEREAL_NVP_("StartTime", startTime));
	archive(CEREAL_NVP_("EndTime", endTime));
	archive(CEREAL_NVP_("DefaultValue", defaultValue));
	archive(CEREAL_NVP_("States", states));
}

template<typename T>
template<class Archive>
void TypedAnimationClip<T>::Load(Archive& archive)
{
	archive(CEREAL_NVP_("BaseInfo", cereal::base_class<AnimationClip>(this)));
	archive(CEREAL_NVP_("StartTime", startTime));
	archive(CEREAL_NVP_("EndTime", endTime));
	archive(CEREAL_NVP_("DefaultValue", defaultValue));
	archive(CEREAL_NVP_("States", states));
	if (startTime < 0.0f)
	{
		startTime = 0.0f;
	}

	if (endTime < 0.0f)
	{
		endTime = 0.0f;
	}

	if (endTime < startTime)
	{
		endTime = startTime;
	}

	if (states.size() < MIN_STATES_COUNT)
	{
		ChangeStatesCount(MIN_STATES_COUNT);
	}
}
