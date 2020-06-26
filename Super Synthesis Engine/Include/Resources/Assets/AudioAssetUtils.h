#ifndef _SSE_AUDIO_ASSET_UTILS_H
#define _SSE_AUDIO_ASSET_UTILS_H

#include <string>

#include "EngineTypeDefs.h"

namespace SSE::Assets::AudioAssetUtils
{
	enum AudioFormat
	{
		AF_RAW,
		AF_WAV,

		AF_NUM_SUPPORTED
	};

	byte* loadAudioFromFile(const std::string& path, st& size);
}

#endif