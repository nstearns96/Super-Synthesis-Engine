#include "Audio/AudioChannel.h"

#include "Logging/Logger.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Audio
	{
		void AudioChannel::setSample(const AudioSample& _sample)
		{
			sample = _sample;
			playCursor = 0;
			isActive = true;
		}

		AudioSample AudioChannel::getSample()
		{
			return sample;
		}

		bool AudioChannel::getIsActive()
		{
			return isActive;
		}

		void AudioChannel::clear()
		{
			sample = {};
			playCursor = 0;
			isActive = false;
		}
	}
}