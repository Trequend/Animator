#pragma once

#include <chrono>
#include <climits>
#include <vector>

#include "Window.hpp"

class PerformanceWindow : public Window
{
public:
	class Data
	{
	public:
		static const size_t HISTORY_SIZE = 100;
	private:
		bool stopped = false;
		
		long long iteration = 1;

		std::vector<float> fpsHistory;
		std::vector<float> frameTimeHistory;
		std::vector<float> updateTimeHistory;
		std::vector<float> renderTimeHistory;
		std::vector<float> uiTimeHistory;

		double fps = 0.0;
		long long frameTime = 0;
		long long updateTime = 0;
		long long renderTime = 0;
		long long uiTime = 0;

		double minFPS = DBL_MAX;
		long long minFrameTime = LLONG_MAX;
		long long minUpdateTime = LLONG_MAX;
		long long minRenderTime = LLONG_MAX;
		long long minUITime = LLONG_MAX;

		double maxFPS = 0;
		long long maxFrameTime = 0;
		long long maxUpdateTime = 0;
		long long maxRenderTime = 0;
		long long maxUITime = 0;

		double totalFPS = 0;
		long long totalFrameTime = 0;
		long long totalUpdateTime = 0;
		long long totalRenderTime = 0;
		long long totalUITime = 0;

		static PerformanceWindow::Data* data;

		static void ThrowIfNoData();
	public:
		Data();
		~Data();

		void Add(
			const std::chrono::milliseconds& updateTime,
			const std::chrono::milliseconds& renderTime,
			const std::chrono::milliseconds& uiTime,
			double fps
		);

		static void Resume();
		static void Stop();

		static bool IsStopped();

		static void Reset();

		static const float* GetFPSHistory();
		static const float* GetFrameTimeHistory();
		static const float* GetUpdateTimeHistory();
		static const float* GetRenderTimeHistory();
		static const float* GetUITimeHistory();

		static double GetFPS();
		static long long GetFrameTime();
		static long long GetUpdateTime();
		static long long GetRenderTime();
		static long long GetUITime();

		static double GetMinFPS();
		static long long GetMinFrameTime();
		static long long GetMinUpdateTime();
		static long long GetMinRenderTime();
		static long long GetMinUITime();

		static double GetMaxFPS();
		static long long GetMaxFrameTime();
		static long long GetMaxUpdateTime();
		static long long GetMaxRenderTime();
		static long long GetMaxUITime();

		static double GetTotalFPS();
		static long long GetTotalFrameTime();
		static long long GetTotalUpdateTime();
		static long long GetTotalRenderTime();
		static long long GetTotalUITime();

		static double GetAverageFPS();
		static double GetAverageFrameTime();
		static double GetAverageUpdateTime();
		static double GetAverageRenderTime();
		static double GetAverageUITime();
	};

	PerformanceWindow();

	void Draw() override;
};
