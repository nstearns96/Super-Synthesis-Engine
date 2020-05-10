#ifndef _SSE_VULKAN_VERTEX_BUFFER_H
#define _SSE_VULKAN_VERTEX_BUFFER_H

#include <vulkan/vulkan.h>

#include <vector>

#include "Model/Vertex.h"

namespace SSE::Vulkan
{
	class VulkanVertexBuffer
	{
	private:
		std::vector<Vertex> vertices;
		VkBuffer vertexBuffer;
		VkDeviceMemory bufferMemory;

	public:
		bool create(const std::vector<Vertex>& _vertices);
		
		void destroy();

		VkBuffer getVertexBuffer();

		unsigned int getVertexCount();
	};
}

#endif