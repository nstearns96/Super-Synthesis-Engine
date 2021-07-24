#include "Time/ScopedTimer.h"

#include "Logging/Logger.h"

namespace SSE
{
	extern Logger gLogger;

	ScopedTimer::ScopedTimer(const std::string& _message)
	{
		timer.startTime();
		message = _message;
	}

	ScopedTimer::~ScopedTimer()
	{
		timer.endTime();
		gLogger.logError(EL_INFO, message + std::to_string(timer.getTime()));
	}
}