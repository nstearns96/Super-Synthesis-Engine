#ifndef _SSE_VULKAN_BUFFER_H
#define _SSE_VULKAN_BUFFER_H

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include "EngineTypeDefs.h"
#include "Vulkan/Memory/VulkanDeviceMemory.h"
#include "Vulkan/Graphics/VulkanImage.h"

namespace SSE::Vulkan
{
	class VulkanBuffer
	{
	private:
		VkBuffer buffer;
		VulkanDeviceMemory bufferMemory;
		u64 size;

	public:
		bool create(u64 _size, bitfield usageFlags, bitfield memoryFlags);

		void destroy();

		VkBuffer getBuffer();
		u64 getSize();
		bool bufferData(void* data, bool useStagingBuffer = true);
		bool copyToImage(VkImage image, const glm::uvec2& dimensions);
	};
}

#endif