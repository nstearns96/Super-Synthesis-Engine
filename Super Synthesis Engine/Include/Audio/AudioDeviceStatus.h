#ifndef _SSE_AUDIO_DEVICE_STATUS_H
#define _SSE_AUDIO_DEVICE_STATUS_H

namespace SSE::Audio
{
	enum AudioDeviceStatus
	{
		ADS_OPEN = 0x1,
		ADS_PLAYING = 0x2,
		ADS_CAPTURE = 0x4,
	};
}

#endif