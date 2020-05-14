#ifndef _SSE_VULKAN_VERTEX_BUFFER_H
#define _SSE_VULKAN_VERTEX_BUFFER_H

#include <vulkan/vulkan.h>

#include <vector>

#include "Vulkan/VulkanBuffer.h"
#include "Model/Vertex.h"

namespace SSE::Vulkan
{
	class VulkanVertexBuffer
	{
	private:
		std::vector<Vertex> vertices;
		std::vector<uint16_t> indices;
#pragma message("TODO: Store vertex and index buffer in same VulkanBuffer object")
		VulkanBuffer vertexBuffer;
		VulkanBuffer indexBuffer;

	public:
		bool create(const std::vector<Vertex>& _vertices, const std::vector<uint16_t> _indices);
		
		void destroy();

		VkBuffer getVertexBuffer();
		VkBuffer getIndexBuffer();

		unsigned int getVertexCount();
		unsigned int getIndexCount();
	};
}

#endif