#ifndef _SSE_AUDIO_MANAGER_H
#define _SSE_AUDIO_MANAGER_H

#include <map>
#include <vector>

#include "Audio/AudioDevice.h"
#include "Audio/AudioChannel.h"
#include "EngineTypeDefs.h"

#define MAX_NUM_CHANNELS 20

typedef st AudioHandle;

namespace SSE
{
	class AudioManager
	{
	private:
		AudioManager() {};

		static Audio::AudioChannel audioChannels[MAX_NUM_CHANNELS];

		static std::map<std::string, Audio::AudioDevice> audioDevices;
		static Audio::AudioDevice* mainOutputDevice;

	public:
		static bool findDevice(const std::string& name, bool isCapture = false);
		
		static AudioHandle play(const Audio::AudioSample& sample);

		static void stop(AudioHandle handle);

		static void clear();

		static void update();
	};
}

#endif