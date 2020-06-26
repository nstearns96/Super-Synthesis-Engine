#include "Vulkan/Graphics/VulkanDepthBuffer.h"

#include "Logging/Logger.h"
#include "Vulkan/Devices/VulkanDeviceManager.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Vulkan
	{
		bool VulkanDepthBuffer::create(const glm::uvec2& dimensions)
		{
			VkFormat depthFormat = VK_FORMAT_UNDEFINED;
			for (st f = 0; f < supportedDepthFormats.size(); ++f)
			{
				VkFormatProperties formatProperties = PHYSICAL_DEVICE.getFormatProperties(supportedDepthFormats[f]);

				if ((formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
				{
					depthFormat = supportedDepthFormats[f];
					break;
				}
			}

			if (depthFormat == VK_FORMAT_UNDEFINED)
			{
				GLOG_CRITICAL("Failed to find suitable format for depth buffer.");
				return false;
			}

			if (image.create(nullptr, dimensions, depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT))
			{
				if (imageView.create(image.getImage(), depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT))
				{
					return true;
				}
			}

			return false;
		}

		void VulkanDepthBuffer::destroy()
		{
			image.destroy();
			imageView.destroy();
		}

		VulkanImageView VulkanDepthBuffer::getImageView()
		{
			return imageView;
		}
	}
}