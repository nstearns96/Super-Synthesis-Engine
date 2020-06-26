#ifndef _SSE_AUDIO_SUPPORT_WAV_H
#define _SSE_AUDIO_SUPPORT_WAV_H

#include "EngineTypeDefs.h"

#define CHUNK_ID(name) ((u32)((name[3] << 24) + (name[2] << 16) + (name[1] << 8) + name[0]))
#define FORMAT_CHUNK_ID CHUNK_ID("fmt ")
#define DATA_CHUNK_ID CHUNK_ID("data")
#define PEAK_CHUNK_ID CHUNK_ID("PEAK")
#define RIFF_CHUNK_ID CHUNK_ID("RIFF")
#define WAVE_CHUNK_ID CHUNK_ID("WAVE")
#define FACT_CHUNK_ID CHUNK_ID("fact")

namespace SSE::Assets::WAV
{
	typedef u32 ChunkId;

	enum ChannelNum
	{
		CN_NONE = 0,
		CN_MONO = 1,
		CN_STEREO = 2
	};

	enum AudioDataType
	{
		ADT_U16 = 0x1,
		ADT_R32 = 0x3
	};

#pragma pack(push, 1)
	struct WavFileHeader
	{
		ChunkId riffHeader;
		u32 chunkSize;
		ChunkId waveHeader;
	};

	struct FormatSubChunk
	{
		ChunkId fmtHeader;
		u32 subChunkSize;
		u16 audioFormat;
		u16 numChannels;
		u32 sampleRate;
		u32 byteRate;
		u16 blockAlign;
		u16 bitsPerSample;
	};

	struct DataSubChunk
	{
		ChunkId dataHeader;
		u32 subChunkSize;
		byte* waveData;
	};

	struct FactSubChunk
	{
		ChunkId factHeader;
		u32 subDataSize;
		byte* subData;
	};

	struct PeakData
	{
		r32 value;
		u32 position;
	};

	struct PeakSubChunk
	{
		ChunkId chunkID;
		u32 chunkDataSize;
		u32 version;
		u32 timeStamp;
		PeakData* peaks;
	};
#pragma pack(pop)

	bool checkWAVHeader(const byte* data);

	byte* extractWaveData(const byte* data, st& size);
}

#endif