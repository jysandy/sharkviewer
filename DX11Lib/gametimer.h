#pragma once
#include <Windows.h>

namespace D3dtut
{
	class GameTimer
	{
	public:
		GameTimer();
		~GameTimer();

		void Reset();
		void Tick();
		float DeltaTime() const;
		float TotalTime() const;
		void Start();
		void Stop();

	private:
		double secondsPerCount;
		float deltaTime;
		long long prevTickCounts;
		long long resetCounts;
		long long elapsedCounts;

		bool stopped;
	};
}