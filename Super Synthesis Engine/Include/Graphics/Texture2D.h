#ifndef _SSE_TEXTURE_2D_H
#define _SSE_TEXTURE_2D_H

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include "Graphics/Bitmap.h"

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
		bool create(Bitmap& bitmap, VkImageTiling _tiling);

		void destroy();

		Vulkan::VulkanImageView getImageView() const;
		VkSampler getSampler() const;
	};
}
#endif