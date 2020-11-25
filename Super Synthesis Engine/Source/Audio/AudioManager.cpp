#include "Audio/AudioManager.h"

#include "Audio/AudioDeviceStatus.h"

#include "Logging/Logger.h"

#pragma message("TODO: Mix audio channels")

namespace SSE
{
	extern Logger gLogger;

	Audio::AudioChannel AudioManager::audioChannels[MAX_NUM_CHANNELS];
	std::map<std::string, Audio::AudioDevice> AudioManager::audioDevices;
	Audio::AudioDevice* AudioManager::mainOutputDevice;

	bool AudioManager::findDevice(const std::string& name, bool isCapture)
	{
		Audio::AudioDevice mainDevice;
		if (mainDevice.create(isCapture))
		{
			audioDevices.emplace(name, mainDevice);
			if (mainOutputDevice == nullptr)
			{
				mainOutputDevice = &audioDevices[name];
			}
			return true;
		}
		else
		{
			return false;
		}
	}

	AudioHandle AudioManager::play(const Audio::AudioSample* sample)
	{
#pragma message("TODO: Get first active channel")
		if (!audioChannels[0].getIsActive())
		{
			audioChannels[0].setSample(*sample);

#pragma message("TODO: Get rid of this once mixing is added and add audio queue control logic")
			mainOutputDevice->queueAudio(*sample);
			mainOutputDevice->unpause();
			return 0;
		}
		else
		{
			GLOG_WARNING("Attempted to play audio on channel that is already playing.");
		}

		return INVALID_HANDLE;
	}

	void AudioManager::stop(AudioHandle handle)
	{
		if (audioChannels[handle].getIsActive())
		{
			audioChannels[handle].clear();

#pragma message("TODO: Get rid of this once mixing is added and add audio queue control logic")
			mainOutputDevice->pause();
			mainOutputDevice->stopAll();
		}
		else
		{
			GLOG_WARNING("Attempted to stop audio on channel that is not playing.");
		}
	}

	void AudioManager::clear()
	{
		for (st c = 0; c < MAX_NUM_CHANNELS; ++c)
		{
			if (audioChannels[c].getIsActive())
			{
				stop(c);
			}
		}

		for (auto& device : audioDevices)
		{
			device.second.destroy();
		}

		audioDevices.clear();
	}

	void AudioManager::update()
	{
#pragma message("TODO: Figure out if an audio channel is outputting silence and, if so, stop it")
#pragma message("TODO: Mix audio and queue it up here")
		for (auto& device : audioDevices)
		{
			device.second.update();
		}
	}
}