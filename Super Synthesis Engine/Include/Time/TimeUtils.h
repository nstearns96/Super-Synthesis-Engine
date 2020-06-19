#ifndef _SSE_TIME_UTILS_H
#define _SSE_TIME_UTILS_H

#include <string>
#include <ctime>
#include <sstream>
#include <chrono>

#include <SDL/SDL_timer.h>

#if defined(_MSC_VER)
#if defined(_WIN64) || defined(_WIN32)
#define LOCAL_TIME(tm, tt) localtime_s(&tm,&tt)
#else
#define LOCAL_TIME(tm,tt) (tm = localtime(&tt))
#define _CRT_SECURE_NO_WARNINGS
#endif
#elif defined(__gnu_linux__) || defined(__APPLE__)
#define LOCAL_TIME(tm,tt) (localtime_r(&tm,&tt))
#else
#define LOCAL_TIME(tm,tt) (tm = localtime(&tt))
#define _CRT_SECURE_NO_WARNINGS
#endif

namespace SSE::TimeUtils
{
	std::string getDate()
	{
		std::ostringstream resultStream;

		std::time_t ctime = std::chrono::system_clock::to_time_t(
			std::chrono::system_clock::now() - std::chrono::hours(24));

		tm time;
		LOCAL_TIME(time, ctime);

		resultStream
			<< ((time.tm_mon < 9) ? "0" : "")
			<< time.tm_mon + 1 << '-'
			<< ((time.tm_mday < 10) ? "0" : "")
			<< time.tm_mday << '-'
			<< time.tm_year + 1900 << ' '
			<< ((time.tm_hour < 10) ? "0" : "")
			<< time.tm_hour << ':'
			<< ((time.tm_min < 10) ? "0" : "")
			<< time.tm_min << ':'
			<< ((time.tm_sec < 10) ? "0" : "")
			<< time.tm_sec;

		return resultStream.str();
	}

	u32 getTicks()
	{
		return SDL_GetTicks();
	}
}

#if defined(_MSC_VER)
#if !defined(_WIN64) && !defined(_WIN32)
#undef _CRT_SECURE_NO_WARNINGS
#endif
#elif !defined(__gnu_linux__) && !defined(__APPLE__)
#undef _CRT_SECURE_NO_WARNINGS
#endif

#endif