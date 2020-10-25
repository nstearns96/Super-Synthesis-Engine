#include "Model/VertexData.h"

namespace SSE
{
	bool VertexData::create(void* _data, st _size, VertexFormat& _format)
	{
		data = _data;
		size = _size;
		format = _format;
		return true;
	}

	void VertexData::destroy()
	{
		delete[] data;
		data = nullptr;

		size = 0;
		format.destroy();
	}

	void* VertexData::getData()
	{
		return data;
	}

	VertexFormat VertexData::getFormat()
	{
		return format;
	}

	st VertexData::getSize()
	{
		return size;
	}
}