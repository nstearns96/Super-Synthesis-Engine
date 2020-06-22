#include "Model/VertexData.h"

namespace SSE
{
	void VertexData::create(void* _data, st _size, VertexFormat& _format)
	{
		data = _data;
		size = _size;
		format = _format;
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