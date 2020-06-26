#ifndef _SSE_AUDIO_CHANNEL_H
#define _SSE_AUDIO_CHANNEL_H

#include "Audio/AudioSample.h"

namespace SSE::Audio
{
	class AudioChannel
	{
	public:
#pragma message("TODO: Add properties like panning, looping, gain, etc")
#pragma message("TODO: Add capability for changes over time like fading out")
#pragma message("TODO: Add proper class interface")
		AudioSample sample;
		st playCursor;
		bool isActive;
	};
}

#endif