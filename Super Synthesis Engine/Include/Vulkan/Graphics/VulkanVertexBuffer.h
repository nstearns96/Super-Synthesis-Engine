#ifndef _SSE_VULKAN_VERTEX_BUFFER_H
#define _SSE_VULKAN_VERTEX_BUFFER_H

#include <vulkan/vulkan.h>

#include <vector>

#include "Vulkan/Memory/VulkanBuffer.h"
#include "Model/Vertex.h"

namespace SSE::Vulkan
{
	class VulkanVertexBuffer
	{
	private:
		std::vector<Vertex> vertices;
		std::vector<u16> indices;
#pragma message("TODO: Store vertex and index buffer in same VulkanBuffer object")
		VulkanBuffer vertexBuffer;
		VulkanBuffer indexBuffer;

	public:
		bool create(const std::vector<Vertex>& _vertices, const std::vector<u16>& _indices);
		
		void destroy();

		VkBuffer getVertexBuffer();
		VkBuffer getIndexBuffer();

		st getVertexCount();
		st getIndexCount();
	};
}

#endif