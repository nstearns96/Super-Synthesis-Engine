#ifndef _SSE_AUDIO_DEVICE_H
#define _SSE_AUDIO_DEVICE_H

#include <SDL/SDL_audio.h>

#include "EngineTypeDefs.h"
#include "Audio/AudioSample.h"

namespace SSE::Audio
{
	class AudioDevice
	{
	private:
		SDL_AudioDeviceID device;
		bitfield status;

		void setStatus(bitfield flags, bool setFlags);
		void setStatus(bitfield flags);

	public:
		bool create(bool isCapture = false);

		void destroy();

		SDL_AudioDeviceID getId();

		bitfield getStatus();

		void queueAudio(AudioSample sample);
		void stopAll();

		void pause();
		void unpause();

		void update();
	};
}

#endif