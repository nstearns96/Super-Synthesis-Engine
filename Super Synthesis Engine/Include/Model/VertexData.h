#ifndef _SSE_VERTEX_DATA_H
#define _SSE_VERTEX_DATA_H

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include "Model/VertexFormat.h"
#include "EngineTypeDefs.h"

namespace SSE
{
	class VertexData
	{
	private:
		void* data;
		st size;
		VertexFormat format;

	public:
		bool create(void* _data, st _size, VertexFormat& _format);

		void destroy();

		void* getData();
		st getSize();
		VertexFormat getFormat();
	};
}

#endif