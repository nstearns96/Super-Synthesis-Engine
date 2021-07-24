#ifndef _SSE_RANDOM_H
#define _SSE_RANDOM_H

#include <random>

#include "EngineTypeDefs.h"

namespace SSE
{
	class Random
	{
	private:
		static std::default_random_engine randomEngine;

		Random() {};
	public:
		static bool init();

		static i32 randInt(i32 min, i32 max);
		static r32 rand(r32 min, r32 max);
	};
}

#endif