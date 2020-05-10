#include "Vulkan/Graphics/VulkanVertexBuffer.h"

#include "Vulkan/Devices/VulkanDeviceManager.h"
#include "Logging/Logger.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Vulkan
	{
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) 
		{
			VkPhysicalDeviceMemoryProperties memProperties;
			vkGetPhysicalDeviceMemoryProperties(PHYSICAL_DEVICE_DEVICE, &memProperties);

			for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) 
			{
				if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
					return i;
				}
			}

			gLogger.logError(ErrorLevel::EL_CRITICAL, "Unable to find valid memory type");
			return -1;
		}

		bool VulkanVertexBuffer::create(const std::vector<Vertex>& _vertices)
		{
			VkBufferCreateInfo bufferInfo{};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = sizeof(Vertex) * _vertices.size();
			bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			if (vkCreateBuffer(LOGICAL_DEVICE_DEVICE, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS) 
			{
				gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to create vertex buffer.");
				return false;
			}

			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(LOGICAL_DEVICE_DEVICE, vertexBuffer, &memRequirements);

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			if (vkAllocateMemory(LOGICAL_DEVICE_DEVICE, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) 
			{
				gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to allocate memory for vertex buffer.");
				return false;
			}

			vkBindBufferMemory(LOGICAL_DEVICE_DEVICE, vertexBuffer, bufferMemory, 0);

			void* data;
			vkMapMemory(LOGICAL_DEVICE_DEVICE, bufferMemory, 0, bufferInfo.size, 0, &data);
			memcpy(data, _vertices.data(), (size_t)bufferInfo.size);
			vkUnmapMemory(LOGICAL_DEVICE_DEVICE, bufferMemory);

			vertices = _vertices;

			return true;
		}

		void VulkanVertexBuffer::destroy()
		{
			vkDestroyBuffer(LOGICAL_DEVICE_DEVICE, vertexBuffer, nullptr);
			vkFreeMemory(LOGICAL_DEVICE_DEVICE, bufferMemory, nullptr);
		}

		VkBuffer VulkanVertexBuffer::getVertexBuffer()
		{
			return vertexBuffer;
		}

		unsigned int VulkanVertexBuffer::getVertexCount()
		{
			return vertices.size();
		}
	}
}