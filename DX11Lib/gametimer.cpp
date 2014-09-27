#include "gametimer.h"

namespace D3dtut
{
	GameTimer::GameTimer()
	{
		this->Reset();
	}

	GameTimer::~GameTimer()
	{

	}

	void GameTimer::Reset()
	{
		deltaTime = 0;
		prevTickCounts = 0;
		elapsedCounts = 0;

		long long frequency;
		QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&frequency));
		secondsPerCount = 1.0 / static_cast<double>(frequency);

		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&this->resetCounts));

		Tick();
	}

	void GameTimer::Tick()
	{
		long long counts;
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&counts));
		
		if (!stopped)
		{
			elapsedCounts += counts - prevTickCounts;
		}
		
		deltaTime = static_cast<float>((counts - prevTickCounts) * secondsPerCount);
		prevTickCounts = counts;

		if (deltaTime < 0)
		{
			deltaTime = 0;
		}

	}

	void GameTimer::Stop()
	{
		stopped = true;
	}

	void GameTimer::Start()
	{
		stopped = false;
	}

	float GameTimer::DeltaTime() const
	{
		return deltaTime;
	}

	float GameTimer::TotalTime() const
	{
		return static_cast<float>(elapsedCounts * secondsPerCount);
	}
}