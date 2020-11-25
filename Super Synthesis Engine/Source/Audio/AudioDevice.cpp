#include "Audio/AudioDevice.h"

#include "Audio/AudioDeviceStatus.h"

#include "Logging/Logger.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Audio
	{
		bool AudioDevice::create(bool isCapture)
		{
			i32 numDevices = SDL_GetNumAudioDevices(isCapture);
			if (numDevices == 0)
			{
				GLOG_CRITICAL("No available audio deivce.");
				return false;
			}

			SDL_AudioSpec requestedAudioSpec, actualAudioSpec;
			SDL_zero(requestedAudioSpec);

#pragma message("TODO: Support different audio formats")
			requestedAudioSpec.freq = 48000;
			requestedAudioSpec.format = AUDIO_F32;
			requestedAudioSpec.channels = 2;
			requestedAudioSpec.samples = 4096;

			const char* deviceName = SDL_GetAudioDeviceName(0, isCapture);

			device = SDL_OpenAudioDevice
			(
				deviceName,
				isCapture,
				&requestedAudioSpec,
				&actualAudioSpec,
				NULL
			);

			if (device != 0)
			{
				status = ADS_OPEN;
				if (isCapture)
				{
					status |= ADS_CAPTURE;
				}

				return true;
			}
			else
			{
				status = 0;
				GLOG_CRITICAL("Failed to open audio device.");
				return false;
			}
		}

		void AudioDevice::destroy()
		{
			if (device != 0)
			{
				SDL_CloseAudioDevice(device);
				status = 0;
			}
			else
			{
				GLOG_WARNING("Attempted to destroy closed audio device.");
			}
		}

		void AudioDevice::queueAudio(AudioSample sample)
		{
			if (status & ADS_CAPTURE)
			{
				GLOG_WARNING("Attempted to queue audio to a capture device.");
				return;
			}

			SDL_QueueAudio(device, sample.getData(), (u32)sample.getSize());
		}

		SDL_AudioDeviceID AudioDevice::getId()
		{
			return device;
		}

		bitfield AudioDevice::getStatus()
		{
			return status;
		}

		void AudioDevice::pause()
		{
			SDL_PauseAudioDevice(device, 1);
			setStatus(ADS_PLAYING, false);
		}

		void AudioDevice::unpause()
		{
			SDL_PauseAudioDevice(device, 0);
			setStatus(ADS_PLAYING, true);
		}

		void AudioDevice::stopAll()
		{
			SDL_ClearQueuedAudio(device);
		}

		void AudioDevice::update()
		{
#pragma message("TODO: Check if we're playing silence and pause if so")
		}

		void AudioDevice::setStatus(bitfield flags, bool setFlags)
		{
			status = setFlags ? (status | flags) : (status & (~flags));
		}

		void AudioDevice::setStatus(bitfield flags)
		{
			status = flags;
		}
	}
}