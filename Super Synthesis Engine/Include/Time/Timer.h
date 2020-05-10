#ifndef _SSE_TIMER_H
#define _SSE_TIMER_H

namespace SSE
{
	class Timer
	{
	private:
		static unsigned long long perfFreq;

		unsigned long long perfCountStart = 0;
		unsigned long long perfCountEnd = 0;
	public:
		static void initTime();

		void startTime();
		void endTime();
		double getTime();
	};
}

#endif