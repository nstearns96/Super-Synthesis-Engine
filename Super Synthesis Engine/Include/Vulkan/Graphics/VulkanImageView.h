#ifndef _SSE_VULKAN_IMAGE_VIEW_H
#define _SSE_VULKAN_IMAGE_VIEW_H

#include <vulkan/vulkan.h>

namespace SSE::Vulkan
{
	class VulkanImageView
	{
	private:
		VkImageView imageView;

	public:
		bool create(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

		void destroy();

		VkImageView getImageView();
	};
}

#endif