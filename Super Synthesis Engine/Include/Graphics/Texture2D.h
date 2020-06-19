#ifndef _SSE_TEXTURE_2D_H
#define _SSE_TEXTURE_2D_H

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include "Vulkan/Graphics/VulkanImage.h"
#include "Vulkan/Graphics/VulkanImageView.h"

namespace SSE::Graphics
{
	class Texture2D
	{
	private:
		Vulkan::VulkanImage image;
		Vulkan::VulkanImageView imageView;
		VkImageTiling tiling;
		VkSampler sampler;

	public:
		bool create(void* data, const glm::uvec2& _dimensions, VkFormat _format, VkImageTiling _tiling);

		void destroy();

		Vulkan::VulkanImageView getImageView();
		VkSampler getSampler();
	};
}
#endif