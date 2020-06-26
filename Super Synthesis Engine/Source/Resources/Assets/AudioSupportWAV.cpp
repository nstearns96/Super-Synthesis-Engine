#include "Resources/Assets/AudioSupportWAV.h"

#include "Logging/Logger.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Assets::WAV
	{
		bool checkWAVHeader(const byte* data)
		{
			WavFileHeader* header = (WavFileHeader*)data;

			return (header->riffHeader == RIFF_CHUNK_ID) && (header->waveHeader == WAVE_CHUNK_ID);
		}

		byte* extractWaveData(const byte* data, st& size)
		{
			const byte* parser = data;

			parser += sizeof(WavFileHeader);
			
			byte* waveData = nullptr;
			ChannelNum numChannels = CN_NONE;
			bool foundData = false;
			while (!foundData)
			{
				switch (*(u32 *)parser)
				{
				case FORMAT_CHUNK_ID:
				{
					FormatSubChunk formatChunk;
					memcpy(&formatChunk, parser, sizeof(formatChunk));
					if (formatChunk.audioFormat == (u16)ADT_R32 && formatChunk.sampleRate == 48000 &&
						formatChunk.numChannels == (u16)CN_STEREO && formatChunk.bitsPerSample == sizeof(r32) * BITS_PER_BYTE)
					{
						numChannels = CN_STEREO;
						parser += sizeof(formatChunk);
					}
					else
					{
						GLOG_CRITICAL("WAV file format not supported.");
						return nullptr;
					}
					break;
				}
				case DATA_CHUNK_ID:
				{
					DataSubChunk dataChunk;
					memcpy(&dataChunk, parser, sizeof(dataChunk) - sizeof(dataChunk.waveData));

					size = (st)dataChunk.subChunkSize;
					waveData = new byte[size];

					parser += sizeof(dataChunk) - sizeof(dataChunk.waveData);
					memcpy(waveData, parser, size);

					foundData = true;
					break;
				}
				case PEAK_CHUNK_ID:
				{
					if (numChannels == CN_NONE)
					{
						GLOG_CRITICAL("PEAK chunk read before format chunk. Failed to read file.");
						return nullptr;
					}

					PeakSubChunk peakChunk;
					memcpy(&peakChunk, parser, sizeof(peakChunk) - sizeof(peakChunk.peaks));
					parser += sizeof(peakChunk) - sizeof(peakChunk.peaks) + (numChannels * sizeof(PeakData));
					break;
				}
				case FACT_CHUNK_ID:
				{
					FactSubChunk factChunk;
					memcpy(&factChunk, parser, sizeof(factChunk) - sizeof(factChunk.subData));
					parser += sizeof(factChunk) - sizeof(factChunk.subData) + factChunk.subDataSize;
					break;
				}
				default:
				{
					GLOG_CRITICAL("Found unsupported chunk while parsing WAV file.");
					return nullptr;
				}
				}
			}

			return waveData;
		}
	}
}