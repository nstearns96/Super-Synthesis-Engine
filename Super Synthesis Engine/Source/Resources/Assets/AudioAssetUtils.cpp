#include "Resources/Assets/AudioAssetUtils.h"

#include "Resources/Assets/AudioSupportWAV.h"
#include "Logging/Logger.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Assets::AudioAssetUtils
	{
		AudioFormat extractAudioFormat(const std::vector<byte>& data)
		{
			if (WAV::checkWAVHeader(data.data()))
			{
				return AF_WAV;
			}
			return AF_RAW;
		}

		byte* loadAudioFromFile(const std::string& path, st& size)
		{
			FileHandle fileHandle;
			if (fileHandle.create(path, FIOM_READ | FIOM_BINARY))
			{
				std::vector<byte> audioData = fileHandle.readIntoVector();

				AudioFormat format = extractAudioFormat(audioData);
				switch (format)
				{
				case AF_RAW:
				default:
				{
					byte* rawData = new byte[audioData.size()];
					memcpy(rawData, audioData.data(), audioData.size());
					size = audioData.size();
					return rawData;
				}
				case AF_WAV:
				{
					return WAV::extractWaveData(audioData.data(), size);
				}
				}
			}

			return nullptr;
		}
	}
}