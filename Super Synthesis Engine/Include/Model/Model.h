#ifndef _SSE_MODEL_H
#define _SSE_MODEL_H

#include "EngineTypeDefs.h"
#include "Model/VertexData.h"
#include "Vulkan/Graphics/VulkanVertexBuffer.h"

#include <vector>

namespace SSE
{
	class Model
	{
	private:
		VertexData vertexData;
		std::vector<u16> indices;
		Vulkan::VulkanVertexBuffer vertexBuffer;

	public:
		bool create(void* data, st size, VertexFormat& format, const std::vector<u16>& _indices);

		void destroy();

		Vulkan::VulkanVertexBuffer getVertexBuffer();
		VertexData getVertexData();
	};
}

#endif