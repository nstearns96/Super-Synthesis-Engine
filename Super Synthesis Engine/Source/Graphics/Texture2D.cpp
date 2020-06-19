#include "Graphics/Texture2D.h"

#include "Logging/Logger.h"
#include "Vulkan/Memory/VulkanBuffer.h"
#include "Resources/Assets/TextureAssetUtils.h"
#include "Vulkan/Devices/VulkanDeviceManager.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Graphics
	{
		bool Texture2D::create(void* data, const glm::uvec2& _dimensions, const VkFormat _format, VkImageTiling _tiling)
		{
			bool result = false;
			if (image.create(_dimensions, _format) && 
				image.transitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL))
			{
				Vulkan::VulkanBuffer imageStagingBuffer;
				if (imageStagingBuffer.create(_dimensions.x * _dimensions.y * 4, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
				{
					if (imageStagingBuffer.bufferData(data, false) &&
						imageStagingBuffer.copyToImage(image) &&
						image.transitionLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) &&
						imageView.create(image.getImage(), _format))
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

						if (vkCreateSampler(LOGICAL_DEVICE_DEVICE, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) 
						{
							gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to create sampler.");
						}
						else
						{
							result = true;
							tiling = _tiling;
						}
					}

					imageStagingBuffer.destroy();
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

		Vulkan::VulkanImageView Texture2D::getImageView()
		{
			return imageView;
		}

		VkSampler Texture2D::getSampler()
		{
			return sampler;
		}
	}
}