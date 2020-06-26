#include "Audio/AudioSample.h"

namespace SSE::Audio
{
	bool AudioSample::create(byte* _data, st _size)
	{
		data = _data;
		size = _size;
		return true;
	}

	void AudioSample::destroy()
	{
		delete[] data;
		size = 0;
	}

	byte* AudioSample::getData()
	{
		return data;
	}

	st AudioSample::getSize()
	{
		return size;
	}
}