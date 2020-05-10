#include "Time/Timer.h"

#include <SDL/SDL_timer.h>

namespace SSE
{
	void Timer::initTime()
	{
		perfFreq = SDL_GetPerformanceFrequency();
	}

	void Timer::startTime()
	{
		perfCountStart = SDL_GetPerformanceCounter();
		perfCountEnd = 0;
	}

	void Timer::endTime()
	{
		perfCountEnd = SDL_GetPerformanceCounter();
	}

	double Timer::getTime()
	{
		if (perfCountEnd < perfCountStart)
		{
			return (double(SDL_GetPerformanceCounter() - 
				perfCountStart)) / perfFreq;
		}
		else
		{
			return (double(perfCountEnd - perfCountEnd)) / perfFreq;
		}
	}
}