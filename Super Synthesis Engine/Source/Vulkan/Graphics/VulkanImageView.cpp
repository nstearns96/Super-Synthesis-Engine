#include "Vulkan/Graphics/VulkanImageView.h"

#include "Logging/Logger.h"
#include "Vulkan/Devices/VulkanDeviceManager.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Vulkan
	{
		bool VulkanImageView::create(VkImage image, VkFormat format)
		{
			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = image;
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = format;
			viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(LOGICAL_DEVICE_DEVICE, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
			{
				gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to create image view.");
				return false;
			}

			return true;
		}

		void VulkanImageView::destroy()
		{
			vkDestroyImageView(LOGICAL_DEVICE_DEVICE, imageView, nullptr);
		}

		VkImageView VulkanImageView::getImageView()
		{
			return imageView;
		}
	}
}