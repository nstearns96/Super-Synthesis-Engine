#ifndef _SSE_VULKAN_IMAGE_H
#define _SSE_VULKAN_IMAGE_H

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include "Vulkan/Memory/VulkanDeviceMemory.h"

namespace SSE::Vulkan
{
	class VulkanImage
	{
	private:
		VkImage image;
		VulkanDeviceMemory imageMemory;
		VkFormat format;
		VkImageLayout layout;

		glm::uvec2 dimensions;

	public:
		bool create(const glm::uvec2& _dimensions, const VkFormat _format);

		void destroy();

		VkImage getImage();

		u32 getWidth();
		u32 getHeight();
		glm::uvec2 getDimensions();

		bool transitionLayout(VkImageLayout newLayout);
	};
}

#endif