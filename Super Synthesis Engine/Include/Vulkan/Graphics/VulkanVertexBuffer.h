#ifndef _SSE_VULKAN_VERTEX_BUFFER_H
#define _SSE_VULKAN_VERTEX_BUFFER_H

#include <vulkan/vulkan.h>

#include <vector>

#include "Model/VertexData.h"

#include "Vulkan/Memory/VulkanBuffer.h"

namespace SSE::Vulkan
{
	class VulkanVertexBuffer
	{
	private:
		VertexData vertices;
		std::vector<u16> indices;
#pragma message("TODO: Store vertex and index buffer in same VulkanBuffer object")
		VulkanBuffer vertexBuffer;
		VulkanBuffer indexBuffer;

	public:
		bool create(VertexData& _vertices, const std::vector<u16>& _indices);
		
		void destroy();

		VkBuffer getVertexBuffer();
		VkBuffer getIndexBuffer();

		st getVertexCount();
		st getIndexCount();
	};
}

#endif