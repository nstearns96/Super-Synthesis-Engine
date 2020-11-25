#ifndef _SSE_VULKAN_DEPTH_BUFFER_H
#define _SSE_VULKAN_DEPTH_BUFFER_H

#include <vector>

#include "Vulkan/Graphics/VulkanImage.h"
#include "Vulkan/Graphics/VulkanImageView.h"

namespace SSE::Vulkan
{
#pragma message("TODO: Support more depth formats")
	static const std::vector<VkFormat> supportedDepthFormats = 
	{
		VK_FORMAT_D32_SFLOAT,
	};

	class VulkanDepthBuffer
	{
	private:
		VulkanImage image;
		VulkanImageView imageView;

	public:
		bool create(const glm::uvec2& dimensions);

		void destroy();

		VulkanImageView getImageView();
	};
}

#endif