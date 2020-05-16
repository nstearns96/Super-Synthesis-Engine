#ifndef _SSE_VULKAN_BUFFER_H
#define _SSE_VULKAN_BUFFER_H

#include <vulkan/vulkan.h>

namespace SSE::Vulkan
{
	class VulkanBuffer
	{
	private:
		VkBuffer buffer;
		VkDeviceMemory bufferMemory;
		uint64_t size;

	public:
		bool create(uint64_t _size, unsigned int usageFlags, unsigned int memoryFlags);

		void destroy();

		VkBuffer getBuffer();
		bool bufferData(void* data, bool useStagingBuffer = true);
	};
}

#endif