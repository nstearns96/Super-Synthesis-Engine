#include "Random/Random.h"

namespace SSE
{
	std::default_random_engine Random::randomEngine;

	bool Random::init()
	{
		std::random_device randomDevice;

		randomEngine.seed(randomDevice());

		return true;
	}

	i32 Random::randInt(i32 min, i32 max)
	{
		return std::uniform_int_distribution<i32>{min, max}(randomEngine);
	}
	
	r32 Random::rand(r32 min, r32 max)
	{
		return std::uniform_real_distribution<r32>{min, max}(randomEngine);
	}
};