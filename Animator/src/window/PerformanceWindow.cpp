#include "PerformanceWindow.hpp"

#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <imgui/imgui.h>

#include "../scene/Scene.hpp"

PerformanceWindow::Data* PerformanceWindow::Data::data = nullptr;

void PerformanceWindow::Data::ThrowIfNoData()
{
	if (data == nullptr)
	{
		throw std::runtime_error("No data");
	}
}

PerformanceWindow::Data::Data()
{
	if (PerformanceWindow::Data::data != nullptr)
	{
		throw std::runtime_error("Data already exists");
	}

	PerformanceWindow::Data::data = this;
	fpsHistory.resize(HISTORY_SIZE);
	frameTimeHistory.resize(HISTORY_SIZE);
	updateTimeHistory.resize(HISTORY_SIZE);
	renderTimeHistory.resize(HISTORY_SIZE);
	uiTimeHistory.resize(HISTORY_SIZE);
	
	for (size_t i = 0; i < HISTORY_SIZE; i++)
	{
		fpsHistory[i] = 0;
		frameTimeHistory[i] = 0;
		updateTimeHistory[i] = 0;
		renderTimeHistory[i] = 0;
		uiTimeHistory[i] = 0;
	}
}

PerformanceWindow::Data::~Data()
{
	PerformanceWindow::Data::data = nullptr;
}

void PerformanceWindow::Data::Add(
	const std::chrono::milliseconds& updateTime,
	const std::chrono::milliseconds& renderTime,
	const std::chrono::milliseconds& uiTime,
	double fps
)
{
	if (stopped)
	{
		return;
	}

	iteration++;

	long long valueUpdateTime = updateTime.count();
	long long valueRenderTime = renderTime.count();
	long long valueUITime = uiTime.count();

	this->fps = fps;
	this->frameTime = valueUpdateTime + valueRenderTime + valueUITime;
	this->updateTime = valueUpdateTime;
	this->renderTime = valueRenderTime;
	this->uiTime = valueUITime;

	if (iteration % 30 == 0)
	{
		fpsHistory.erase(fpsHistory.begin());
		frameTimeHistory.erase(frameTimeHistory.begin());
		updateTimeHistory.erase(updateTimeHistory.begin());
		renderTimeHistory.erase(renderTimeHistory.begin());
		uiTimeHistory.erase(uiTimeHistory.begin());

		fpsHistory.push_back(float(this->fps));
		frameTimeHistory.push_back(float(this->frameTime));
		updateTimeHistory.push_back(float(this->updateTime));
		renderTimeHistory.push_back(float(this->renderTime));
		uiTimeHistory.push_back(float(this->uiTime));
	}

	minFPS = std::min(minFPS, fps);
	minFrameTime = std::min(minFrameTime, frameTime);
	minUpdateTime = std::min(minUpdateTime, valueUpdateTime);
	minRenderTime = std::min(minRenderTime, valueRenderTime);
	minUITime = std::min(minUITime, valueUITime);

	maxFPS = std::max(maxFPS, fps);
	maxFrameTime = std::max(maxFrameTime, frameTime);
	maxUpdateTime = std::max(maxUpdateTime, valueUpdateTime);
	maxRenderTime = std::max(maxRenderTime, valueRenderTime);
	maxUITime = std::max(maxUITime, valueUITime);

	totalFPS += fps;
	totalFrameTime += frameTime;
	totalUpdateTime += valueUpdateTime;
	totalRenderTime += valueRenderTime;
	totalUITime += valueUITime;
}

void PerformanceWindow::Data::Resume()
{
	ThrowIfNoData();
	data->stopped = false;
}

void PerformanceWindow::Data::Stop()
{
	ThrowIfNoData();
	data->stopped = true;
}

bool PerformanceWindow::Data::IsStopped()
{
	ThrowIfNoData();
	return data->stopped;
}

void PerformanceWindow::Data::Reset()
{
	if (data == nullptr)
	{
		return;
	}

	data->stopped = false;

	data->iteration = 1;

	data->fps = 0.0;
	data->frameTime = 0;
	data->updateTime = 0;
	data->renderTime = 0;
	data->uiTime = 0;

	data->minFPS = DBL_MAX;
	data->minFrameTime = LLONG_MAX;
	data->minUpdateTime = LLONG_MAX;
	data->minRenderTime = LLONG_MAX;
	data->minUITime = LLONG_MAX;

	data->maxFPS = 0;
	data->maxFrameTime = 0;
	data->maxUpdateTime = 0;
	data->maxRenderTime = 0;
	data->maxUITime = 0;

	data->totalFPS = 0;
	data->totalFrameTime = 0;
	data->totalUpdateTime = 0;
	data->totalRenderTime = 0;
	data->totalUITime = 0;
}

const float* PerformanceWindow::Data::GetFPSHistory()
{
	ThrowIfNoData();
	return &data->fpsHistory[0];
}

const float* PerformanceWindow::Data::GetFrameTimeHistory()
{
	ThrowIfNoData();
	return &data->frameTimeHistory[0];
}

const float* PerformanceWindow::Data::GetUpdateTimeHistory()
{
	ThrowIfNoData();
	return &data->updateTimeHistory[0];
}

const float* PerformanceWindow::Data::GetRenderTimeHistory()
{
	ThrowIfNoData();
	return &data->renderTimeHistory[0];
}

const float* PerformanceWindow::Data::GetUITimeHistory()
{
	ThrowIfNoData();
	return &data->uiTimeHistory[0];
}

double PerformanceWindow::Data::GetFPS()
{
	ThrowIfNoData();
	return data->fps;
}

long long PerformanceWindow::Data::GetFrameTime()
{
	ThrowIfNoData();
	return data->frameTime;
}

long long PerformanceWindow::Data::GetUpdateTime()
{
	ThrowIfNoData();
	return data->updateTime;
}

long long PerformanceWindow::Data::GetRenderTime()
{
	ThrowIfNoData();
	return data->renderTime;
}

long long PerformanceWindow::Data::GetUITime()
{
	ThrowIfNoData();
	return data->uiTime;
}

double PerformanceWindow::Data::GetMinFPS()
{
	ThrowIfNoData();
	return data->minFPS;
}

long long PerformanceWindow::Data::GetMinFrameTime()
{
	ThrowIfNoData();
	return data->minFrameTime;
}

long long PerformanceWindow::Data::GetMinUpdateTime()
{
	ThrowIfNoData();
	return data->minUpdateTime;
}

long long PerformanceWindow::Data::GetMinRenderTime()
{
	ThrowIfNoData();
	return data->minRenderTime;
}

long long PerformanceWindow::Data::GetMinUITime()
{
	ThrowIfNoData();
	return data->minUITime;
}

double PerformanceWindow::Data::GetMaxFPS()
{
	ThrowIfNoData();
	return data->maxFPS;
}

long long PerformanceWindow::Data::GetMaxFrameTime()
{
	ThrowIfNoData();
	return data->maxFrameTime;
}

long long PerformanceWindow::Data::GetMaxUpdateTime()
{
	ThrowIfNoData();
	return data->maxUpdateTime;
}

long long PerformanceWindow::Data::GetMaxRenderTime()
{
	ThrowIfNoData();
	return data->maxRenderTime;
}

long long PerformanceWindow::Data::GetMaxUITime()
{
	ThrowIfNoData();
	return data->maxUITime;
}

double PerformanceWindow::Data::GetTotalFPS()
{
	ThrowIfNoData();
	return data->totalFPS;
}

long long PerformanceWindow::Data::GetTotalFrameTime()
{
	ThrowIfNoData();
	return data->totalFrameTime;
}

long long PerformanceWindow::Data::GetTotalUpdateTime()
{
	ThrowIfNoData();
	return data->totalUpdateTime;
}

long long PerformanceWindow::Data::GetTotalRenderTime()
{
	ThrowIfNoData();
	return data->totalRenderTime;
}

long long PerformanceWindow::Data::GetTotalUITime()
{
	ThrowIfNoData();
	return data->totalUITime;
}

double PerformanceWindow::Data::GetAverageFPS()
{
	ThrowIfNoData();
	return data->totalFPS / data->iteration;
}

double PerformanceWindow::Data::GetAverageFrameTime()
{
	ThrowIfNoData();
	return double(data->totalFrameTime) / data->iteration;
}

double PerformanceWindow::Data::GetAverageUpdateTime()
{
	ThrowIfNoData();
	return double(data->totalUpdateTime) / data->iteration;
}

double PerformanceWindow::Data::GetAverageRenderTime()
{
	ThrowIfNoData();
	return double(data->totalRenderTime) / data->iteration;
}

double PerformanceWindow::Data::GetAverageUITime()
{
	ThrowIfNoData();
	return double(data->totalUITime) / data->iteration;
}

PerformanceWindow::PerformanceWindow() : Window("Performance", false) { }

void PerformanceWindow::Draw()
{
	ImGui::Begin(GetName().c_str());

	ImGui::Text(
		"FPS: %lld (min: %lld; avg: %lld; max: %lld)",
		std::llround(PerformanceWindow::Data::GetFPS()),
		std::llround(PerformanceWindow::Data::GetMinFPS()),
		std::llround(PerformanceWindow::Data::GetAverageFPS()),
		std::llround(PerformanceWindow::Data::GetMaxFPS())
	);
	ImGui::Text(
		"Frame time: %lld (min: %lld, avg: %lld; max: %lld)",
		PerformanceWindow::Data::GetFrameTime(),
		PerformanceWindow::Data::GetMinFrameTime(),
		std::llround(PerformanceWindow::Data::GetAverageFrameTime()),
		PerformanceWindow::Data::GetMaxFrameTime()
	);
	ImGui::Text(
		"Update time: %lld (min: %lld, avg: %lld; max: %lld)",
		PerformanceWindow::Data::GetUpdateTime(),
		PerformanceWindow::Data::GetMinUpdateTime(),
		std::llround(PerformanceWindow::Data::GetAverageUpdateTime()),
		PerformanceWindow::Data::GetMaxUpdateTime()
	);
	ImGui::Text(
		"Render time: %lld (min: %lld, avg: %lld; max: %lld)",
		PerformanceWindow::Data::GetRenderTime(),
		PerformanceWindow::Data::GetMinRenderTime(),
		std::llround(PerformanceWindow::Data::GetAverageRenderTime()),
		PerformanceWindow::Data::GetMaxRenderTime()
	);
	ImGui::Text(
		"UI time: %lld (min: %lld, avg: %lld; max: %lld)",
		PerformanceWindow::Data::GetUITime(),
		PerformanceWindow::Data::GetMinUITime(),
		std::llround(PerformanceWindow::Data::GetAverageUITime()),
		PerformanceWindow::Data::GetMaxUITime()
	);

	Scene& scene = Scene::Current();
	ImGui::Text("Verticies: %zu", scene.GetPrism().GetVerticies().size());

	ImGui::PlotLines("FPS", PerformanceWindow::Data::GetFPSHistory(), PerformanceWindow::Data::HISTORY_SIZE);
	ImGui::PlotLines("Frame time", PerformanceWindow::Data::GetFrameTimeHistory(), PerformanceWindow::Data::HISTORY_SIZE);
	ImGui::PlotLines("Update time", PerformanceWindow::Data::GetUpdateTimeHistory(), PerformanceWindow::Data::HISTORY_SIZE);
	ImGui::PlotLines("Render time", PerformanceWindow::Data::GetRenderTimeHistory(), PerformanceWindow::Data::HISTORY_SIZE);
	ImGui::PlotLines("UI time", PerformanceWindow::Data::GetUITimeHistory(), PerformanceWindow::Data::HISTORY_SIZE);

	if (ImGui::Button("Reset"))
	{
		PerformanceWindow::Data::Reset();
	}

	ImGui::SameLine();
	if (PerformanceWindow::Data::IsStopped())
	{
		if (ImGui::Button("Resume"))
		{
			PerformanceWindow::Data::Resume();
		}
	}
	else
	{
		if (ImGui::Button("Stop"))
		{
			PerformanceWindow::Data::Stop();
		}
	}

	ImGui::End();
}
