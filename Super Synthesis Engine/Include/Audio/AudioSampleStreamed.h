#ifndef _SSE_AUDIO_SAMPLE_STREAMED_H
#define _SSE_AUDIO_SAMPLE_STREAMED_H

#include <vector>

#include "EngineTypeDefs.h"

#include "Fios/FileHandle.h"

namespace SSE::Audio
{
	class AudioSampleStreamed
	{
	private:
		std::vector<byte> buffer;
		FileHandle streamSource;

	public:
		bool create(FileHandle _streamSource);

		void destroy();
	};
}

#endif