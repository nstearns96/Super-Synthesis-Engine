#include "Vulkan/Graphics/VulkanVertexBuffer.h"

#include "Vulkan/Devices/VulkanDeviceManager.h"
#include "Logging/Logger.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Vulkan
	{
		bool VulkanVertexBuffer::create(VertexData& _vertices, const std::vector<u16>& _indices)
		{
			bool result = false;
			if(vertexBuffer.create(_vertices.getSize(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
			{
				if (indexBuffer.create(sizeof(u16) * _indices.size(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
				{
					if (vertexBuffer.bufferData(_vertices.getData()) && indexBuffer.bufferData((void *) _indices.data()))
					{
						vertices = _vertices;
						indices = _indices;
						result = true;
					}
					else
					{
						vertexBuffer.destroy();
						indexBuffer.destroy();
					}
				}
				else
				{
					vertexBuffer.destroy();
				}
			}

			return result;
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

		st VulkanVertexBuffer::getVertexCount()
		{
			return vertices.getSize()/vertices.getFormat().getFormatStride();
		}

		st VulkanVertexBuffer::getIndexCount()
		{
			return indices.size();
		}
	}
}