#include "Graphics/Texture2D.h"

#include "Logging/Logger.h"

#include "Resources/Assets/TextureAssetUtils.h"

#include "Vulkan/Devices/VulkanDeviceManager.h"

#include "Vulkan/Memory/VulkanBuffer.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Graphics
	{
		bool Texture2D::create(Bitmap& bitmap, VkImageTiling _tiling)
		{
			bool result = false;
			if (bitmap.getFormat() != VK_FORMAT_B8G8R8A8_UINT)
			{
				BitmapFormatTransitionParams params = {};
				params.newFormat = VK_FORMAT_B8G8R8A8_UINT;
				params.channelParams[CHANNEL_GREEN].destinationChannel = CHANNEL_GREEN;
				params.channelParams[CHANNEL_BLUE].destinationChannel = CHANNEL_BLUE;
				params.channelParams[CHANNEL_ALPHA].destinationChannel = CHANNEL_ALPHA;
				params.channelParams[CHANNEL_ALPHA].constant = UINT_MAX;
				if (!bitmap.transitionFormat(params))
				{
					GLOG_CRITICAL("Could not create texture. Failed to transition input bitmap.");
					return false;
				}
			}

			if (image.create(bitmap.getData(), bitmap.getDimensions(), VK_FORMAT_B8G8R8A8_SRGB, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT))
			{
				if (image.transitionLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) &&
					imageView.create(image.getImage(), VK_FORMAT_B8G8R8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT))
				{
					VkSamplerCreateInfo samplerInfo{};
					samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
					samplerInfo.magFilter = VK_FILTER_LINEAR;
					samplerInfo.minFilter = VK_FILTER_LINEAR;
					samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
					samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
					samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
					samplerInfo.anisotropyEnable = VK_TRUE;
					samplerInfo.maxAnisotropy = 16.0f;
					samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
					samplerInfo.unnormalizedCoordinates = VK_FALSE;
					samplerInfo.compareEnable = VK_FALSE;
					samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
					samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
					samplerInfo.mipLodBias = 0.0f;
					samplerInfo.minLod = 0.0f;
					samplerInfo.maxLod = 0.0f;

					if (vkCreateSampler(LOGICAL_DEVICE_DEVICE, &samplerInfo, nullptr, &sampler) == VK_SUCCESS)
					{
						result = true;
						tiling = _tiling;
					}
					else
					{
						GLOG_CRITICAL("Failed to create sampler.");
						imageView.destroy();
						image.destroy();
					}
				}
				else
				{
					image.destroy();
				}
			}

			return result;
		}

		void Texture2D::destroy()
		{
			image.destroy();
			vkDestroyImageView(LOGICAL_DEVICE_DEVICE, imageView.getImageView(), nullptr);
			vkDestroySampler(LOGICAL_DEVICE_DEVICE, sampler, nullptr);
		}

		Vulkan::VulkanImageView Texture2D::getImageView() const
		{
			return imageView;
		}

		VkSampler Texture2D::getSampler() const
		{
			return sampler;
		}
	}
}