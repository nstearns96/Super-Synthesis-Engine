#ifndef _SSE_AUDIO_SAMPLE_H
#define _SSE_AUDIO_SAMPLE_H

#include "EngineTypeDefs.h"

namespace SSE::Audio
{
	class AudioSample
	{
	private:
		byte* data;
		st size;

	public:
		bool create(byte* _data, st _size);

		void destroy();

		byte* getData();

		st getSize();
	};
}

#endif