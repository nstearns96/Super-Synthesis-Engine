#include "vulkan/Memory/VulkanBuffer.h"

#include "Vulkan/Devices/VulkanDeviceManager.h"
#include "Logging/Logger.h"
#include "Vulkan/VulkanCommandPool.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Vulkan
	{
		bool VulkanBuffer::create(st _size, bitfield usageFlags, bitfield memoryFlags)
		{
			VkBufferCreateInfo bufferInfo{};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = _size;
			bufferInfo.usage = usageFlags;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			if (vkCreateBuffer(LOGICAL_DEVICE_DEVICE, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
			{
				GLOG_CRITICAL("Failed to create buffer.");
				return false;
			}

			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(LOGICAL_DEVICE_DEVICE, buffer, &memRequirements);

			bufferMemory.create(memRequirements, memoryFlags);

			vkBindBufferMemory(LOGICAL_DEVICE_DEVICE, buffer, bufferMemory.getMemory(), 0);

			size = _size;

			return true;
		}

		void VulkanBuffer::destroy()
		{
			vkDestroyBuffer(LOGICAL_DEVICE_DEVICE, buffer, nullptr);
			bufferMemory.destroy();
			size = 0;
		}

		VkBuffer VulkanBuffer::getBuffer()
		{
			return buffer;
		}

		u64 VulkanBuffer::getSize()
		{
			return size;
		}

		bool VulkanBuffer::bufferData(void* data, bool useStagingBuffer)
		{
			if (useStagingBuffer)
			{
				bool result = false;
				VulkanBuffer stagingBuffer;
				if (stagingBuffer.create(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
				{
					if (stagingBuffer.bufferData(data, false))
					{
						VulkanCommandPool copyCommandPool;
						if (copyCommandPool.create())
						{
							if (copyCommandPool.allocateBuffers(1) &&
								copyCommandPool.beginBuffers())
							{
								VkCommandBuffer& copyCommand = copyCommandPool.getNewCommandBuffer(0);

								VkBufferCopy copyRegion{};
								copyRegion.srcOffset = 0;
								copyRegion.dstOffset = 0;
								copyRegion.size = size;
								vkCmdCopyBuffer(copyCommand, stagingBuffer.getBuffer(), buffer, 1, &copyRegion);

								if (copyCommandPool.endBuffers() &&
									copyCommandPool.execute())
								{
									result = true;
								}
							}

							copyCommandPool.destroy();
						}
					}

					stagingBuffer.destroy();
				}

				return result;
			}
			else
			{
				return bufferMemory.mapMemory(data, size);
			}
		}

		bool VulkanBuffer::copyToImage(VkImage image, const glm::uvec2& dimensions)
		{
			bool result = false;

			VulkanCommandPool copyCommandPool;
			if (copyCommandPool.create())
			{
				if (copyCommandPool.allocateBuffers(1) &&
					copyCommandPool.beginBuffers())
				{
					VkCommandBuffer& copyCommand = copyCommandPool.getNewCommandBuffer(0);

					VkBufferImageCopy region{};
					region.bufferOffset = 0;
					region.bufferRowLength = 0;
					region.bufferImageHeight = 0;
					region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					region.imageSubresource.mipLevel = 0;
					region.imageSubresource.baseArrayLayer = 0;
					region.imageSubresource.layerCount = 1;
					region.imageOffset = { 0, 0, 0 };
					region.imageExtent = { dimensions.x, dimensions.y, 1 };

					vkCmdCopyBufferToImage
					(
						copyCommand,
						buffer,
						image,
						VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						1,
						&region
					);

					if (copyCommandPool.endBuffers() &&
						copyCommandPool.execute())
					{
						result = true;
					}
				}

				copyCommandPool.destroy();
			}

			return result;
		}
	}
}
