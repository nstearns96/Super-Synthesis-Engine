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

	r64 Timer::getTime()
	{
		if (perfCountEnd < perfCountStart)
		{
			return ((r64) (SDL_GetPerformanceCounter() - 
				perfCountStart)) / perfFreq;
		}
		else
		{
			return ((r64)(perfCountEnd - perfCountEnd)) / perfFreq;
		}
	}
}