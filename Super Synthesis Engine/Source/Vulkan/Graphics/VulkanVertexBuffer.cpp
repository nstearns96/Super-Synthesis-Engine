#include "Vulkan/Graphics/VulkanVertexBuffer.h"

#include "Vulkan/Devices/VulkanDeviceManager.h"
#include "Logging/Logger.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Vulkan
	{
		bool VulkanVertexBuffer::create(const std::vector<Vertex>& _vertices, const std::vector<uint16_t> _indices)
		{
			if(!vertexBuffer.create((void *)_vertices.data(), sizeof(Vertex) * _vertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
				|| !indexBuffer.create((void *)_indices.data(), sizeof(uint16_t) * _indices.size(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
			{
				return false;
			}

			vertices = _vertices;
			indices = _indices;

			return true;
		}

		void VulkanVertexBuffer::destroy()
		{
			vertexBuffer.destroy();
			indexBuffer.destroy();
		}

		VkBuffer VulkanVertexBuffer::getVertexBuffer()
		{
			return vertexBuffer.getBuffer();
		}

		VkBuffer VulkanVertexBuffer::getIndexBuffer()
		{
			return indexBuffer.getBuffer();
		}

		unsigned int VulkanVertexBuffer::getVertexCount()
		{
			return vertices.size();
		}

		unsigned int VulkanVertexBuffer::getIndexCount()
		{
			return indices.size();
		}
	}
}