#ifndef _SSE_AUDIO_CHANNEL_H
#define _SSE_AUDIO_CHANNEL_H

#include "Audio/AudioSample.h"

namespace SSE::Audio
{
	class AudioChannel
	{
	private:
#pragma message("TODO: Add properties like panning, looping, gain, etc")
#pragma message("TODO: Add capability for changes over time like fading out")
		AudioSample sample;
		st playCursor;
		bool isActive;

	public:
		void setSample(const AudioSample& sample);
		AudioSample getSample();

		bool getIsActive();

		void clear();
	};
}

#endif