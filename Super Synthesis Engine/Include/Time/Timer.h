#ifndef _SSE_TIMER_H
#define _SSE_TIMER_H

#include "EngineTypeDefs.h"

namespace SSE
{
	class Timer
	{
	private:
		static u64 perfFreq;

		u64 perfCountStart = 0;
		u64 perfCountEnd = 0;
	public:
		static void initTime();

		void startTime();
		void endTime();
		r64 getTime();
	};
}

#endif