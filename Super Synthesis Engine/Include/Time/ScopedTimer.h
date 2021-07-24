#ifndef _SSE_SCOPED_TIMER_H
#define _SSE_SCOPED_TIMER_H

#include <string>

#include "Time/Timer.h"

namespace SSE
{
	class ScopedTimer
	{
	private:
		Timer timer;
		std::string message;

	public:
		ScopedTimer(const std::string& _message);
		~ScopedTimer();
	};
}

#endif