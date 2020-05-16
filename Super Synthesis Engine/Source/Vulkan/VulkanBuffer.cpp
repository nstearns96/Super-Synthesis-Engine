#include "vulkan/VulkanBuffer.h"

#include "Vulkan/Devices/VulkanDeviceManager.h"
#include "Logging/Logger.h"
#include "vulkan/VulkanCommandPool.h"

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

		bool VulkanBuffer::create(uint64_t _size, unsigned int usageFlags, unsigned int memoryFlags)
		{
			VkBufferCreateInfo bufferInfo{};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = _size;
			bufferInfo.usage = usageFlags | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			if (vkCreateBuffer(LOGICAL_DEVICE_DEVICE, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
			{
				gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to create buffer.");
				return false;
			}

			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(LOGICAL_DEVICE_DEVICE, buffer, &memRequirements);

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, memoryFlags);

			if (vkAllocateMemory(LOGICAL_DEVICE_DEVICE, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
			{
				gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to allocate memory for buffer.");
				return false;
			}

			vkBindBufferMemory(LOGICAL_DEVICE_DEVICE, buffer, bufferMemory, 0);

			size = _size;

			return true;
		}

		void VulkanBuffer::destroy()
		{
			vkDestroyBuffer(LOGICAL_DEVICE_DEVICE, buffer, nullptr);
			vkFreeMemory(LOGICAL_DEVICE_DEVICE, bufferMemory, nullptr);
			size = 0;
		}

		VkBuffer VulkanBuffer::getBuffer()
		{
			return buffer;
		}

		bool VulkanBuffer::bufferData(void* data, bool useStagingBuffer)
		{
			if (useStagingBuffer)
			{
				VkBuffer stagingBuffer;
				VkDeviceMemory stagingBufferMemory;

				VkBufferCreateInfo stagingBufferInfo{};
				stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
				stagingBufferInfo.size = size;
				stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
				stagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

				if (vkCreateBuffer(LOGICAL_DEVICE_DEVICE, &stagingBufferInfo, nullptr, &stagingBuffer) != VK_SUCCESS)
				{
					gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to create staging buffer.");
					return false;
				}

				VkMemoryRequirements stagingMemRequirements;
				vkGetBufferMemoryRequirements(LOGICAL_DEVICE_DEVICE, stagingBuffer, &stagingMemRequirements);

				VkMemoryAllocateInfo stagingAllocInfo{};
				stagingAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				stagingAllocInfo.allocationSize = stagingMemRequirements.size;
				stagingAllocInfo.memoryTypeIndex = findMemoryType(stagingMemRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

				if (vkAllocateMemory(LOGICAL_DEVICE_DEVICE, &stagingAllocInfo, nullptr, &stagingBufferMemory) != VK_SUCCESS)
				{
					gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to allocate memory for staging buffer.");
					return false;
				}

				vkBindBufferMemory(LOGICAL_DEVICE_DEVICE, stagingBuffer, stagingBufferMemory, 0);

				void* mappedMemory;
				vkMapMemory(LOGICAL_DEVICE_DEVICE, stagingBufferMemory, 0, size, 0, &mappedMemory);
				memcpy(mappedMemory, data, (size_t)size);
				vkUnmapMemory(LOGICAL_DEVICE_DEVICE, stagingBufferMemory);

				VulkanCommandPool copyCommandPool;
				if (!copyCommandPool.create() || !copyCommandPool.allocateBuffers(1))
				{
					return false;
				}

				VkCommandBuffer& copyCommand = copyCommandPool.getNewCommandBuffer(0);

				VkCommandBufferBeginInfo beginInfo{};
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

				vkBeginCommandBuffer(copyCommand, &beginInfo);

				VkBufferCopy copyRegion{};
				copyRegion.srcOffset = 0;
				copyRegion.dstOffset = 0;
				copyRegion.size = size;
				vkCmdCopyBuffer(copyCommand, stagingBuffer, buffer, 1, &copyRegion);

				vkEndCommandBuffer(copyCommand);

				VkSubmitInfo submitInfo{};
				submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
				submitInfo.commandBufferCount = 1;
				submitInfo.pCommandBuffers = &copyCommand;

				VulkanLogicalDevice logicalDevice = LOGICAL_DEVICE;
				logicalDevice.submit(submitInfo, nullptr);
				logicalDevice.waitGraphicsIdle();

				copyCommandPool.destroy();

				vkDestroyBuffer(LOGICAL_DEVICE_DEVICE, stagingBuffer, nullptr);
				vkFreeMemory(LOGICAL_DEVICE_DEVICE, stagingBufferMemory, nullptr);
			}
			else
			{
				void* mappedMemory;
				vkMapMemory(LOGICAL_DEVICE_DEVICE, bufferMemory, 0, size, 0, &mappedMemory);
				memcpy(mappedMemory, data, (size_t)size);
				vkUnmapMemory(LOGICAL_DEVICE_DEVICE, bufferMemory);
			}
			return true;
		}
	}
}
