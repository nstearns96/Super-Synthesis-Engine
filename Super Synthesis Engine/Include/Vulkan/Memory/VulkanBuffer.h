#ifndef _SSE_VULKAN_BUFFER_H
#define _SSE_VULKAN_BUFFER_H

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include "EngineTypeDefs.h"

#include "Vulkan/Graphics/VulkanImage.h"

#include "Vulkan/Memory/VulkanDeviceMemory.h"

namespace SSE::Vulkan
{
	class VulkanBuffer
	{
	private:
		VkBuffer buffer;
		VulkanDeviceMemory bufferMemory;
		st size;

	public:
		bool create(st _size, bitfield usageFlags, bitfield memoryFlags);

		void destroy();

		VkBuffer getBuffer();
		st getSize();
		bool bufferData(void* data, bool useStagingBuffer = true);
		bool copyToImage(VkImage image, const glm::uvec2& dimensions);
	};
}

#endif