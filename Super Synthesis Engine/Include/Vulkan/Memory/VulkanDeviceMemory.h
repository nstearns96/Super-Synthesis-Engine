#ifndef _SSE_VULKAN_DEVICE_MEMORY_H
#define _SSE_VULKAN_DEVICE_MEMORY_H

#include <vulkan/vulkan.h>

#include "EngineTypeDefs.h"

namespace SSE::Vulkan
{
	class VulkanDeviceMemory
	{
	private:
		VkDeviceMemory deviceMemory;
		
		static u32 findMemoryType(bitfield typeFilter, VkMemoryPropertyFlags properties);
	public:
		bool create(const VkMemoryRequirements& memRequirements, bitfield memoryFlags);

		void destroy();

		VkDeviceMemory getMemory();

		bool mapMemory(void* data, VkDeviceSize size);
	};
}

#endif