#include "Vulkan/Memory/VulkanDeviceMemory.h"

#include "Logging/Logger.h"
#include "Vulkan/Devices/VulkanDeviceManager.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Vulkan
	{
		u32 VulkanDeviceMemory::findMemoryType(bitfield typeFilter, VkMemoryPropertyFlags properties)
		{
			VkPhysicalDeviceMemoryProperties memProperties;
			vkGetPhysicalDeviceMemoryProperties(PHYSICAL_DEVICE_DEVICE, &memProperties);

			for (u32 i = 0; i < memProperties.memoryTypeCount; i++)
			{
				if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) 
				{
					return i;
				}
			}

			GLOG_CRITICAL("Unable to find valid memory type");
			return UINT_MAX;
		}

		bool VulkanDeviceMemory::create(const VkMemoryRequirements& memRequirements, bitfield memoryFlags)
		{
			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, memoryFlags);

			if (vkAllocateMemory(LOGICAL_DEVICE_DEVICE, &allocInfo, nullptr, &deviceMemory) != VK_SUCCESS)
			{
				GLOG_CRITICAL("Failed to allocate memory for buffer.");
				return false;
			}

			return true;
		}

		void VulkanDeviceMemory::destroy()
		{
			vkFreeMemory(LOGICAL_DEVICE_DEVICE, deviceMemory, nullptr);
		}

		VkDeviceMemory VulkanDeviceMemory::getMemory()
		{
			return deviceMemory;
		}

		bool VulkanDeviceMemory::mapMemory(void* data, VkDeviceSize size)
		{
			void* mappedMemory;
			if (vkMapMemory(LOGICAL_DEVICE_DEVICE, deviceMemory, 0, size, 0, &mappedMemory) != VK_SUCCESS)
			{
				return false;
			}
			memcpy(mappedMemory, data, (st)size);
			vkUnmapMemory(LOGICAL_DEVICE_DEVICE, deviceMemory);

			return true;
		}
	}
}