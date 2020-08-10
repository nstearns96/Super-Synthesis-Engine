#include "vulkan/Graphics/VulkanImage.h"

#include <map>

#include <glm/glm.hpp>

#include "Vulkan/Devices/VulkanDeviceManager.h"
#include "Logging/Logger.h"
#include "Vulkan/VulkanCommandPool.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Vulkan
	{
#pragma message("TODO: Expand these into arrays")
		static const std::map<VkImageLayout, VkAccessFlags> layoutToAccessMap =
		{
			{VK_IMAGE_LAYOUT_UNDEFINED, NULL},
			{VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_ACCESS_TRANSFER_WRITE_BIT},
			{VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_ACCESS_SHADER_READ_BIT},
			{VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT}
		};

		static const std::map<VkImageLayout, VkPipelineStageFlags> layoutToPipelineStageMap =
		{
			{VK_IMAGE_LAYOUT_UNDEFINED, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT},
			{VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT},
			{VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT},
			{VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT}
		};

		bool VulkanImage::create(void* data, const glm::uvec2& _dimensions, const VkFormat _format, VkImageUsageFlags usage)
		{
			bool result = false;

			VkImageCreateInfo imageInfo{};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = _dimensions.x;
			imageInfo.extent.height = _dimensions.y;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
#pragma message("TODO: Support different image formats")
			imageInfo.format = _format;
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = usage;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
#pragma message("TODO: Add support for MSAA")
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.flags = 0;

			if (vkCreateImage(LOGICAL_DEVICE_DEVICE, &imageInfo, nullptr, &image) == VK_SUCCESS)
			{
				VkMemoryRequirements memRequirements;
				vkGetImageMemoryRequirements(LOGICAL_DEVICE_DEVICE, image, &memRequirements);

				if (imageMemory.create(memRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
				{
					layout = VK_IMAGE_LAYOUT_UNDEFINED;

					vkBindImageMemory(LOGICAL_DEVICE_DEVICE, image, imageMemory.getMemory(), 0);

					dimensions = _dimensions;
					format = _format;

#pragma message("TODO: Better support for hardware-level image buffers i.e. depth buffers, stencil buffers")
					if (data != nullptr)
					{
						VulkanBuffer imageStagingBuffer;
						if (transitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) &&
							imageStagingBuffer.create((st)_dimensions.x * _dimensions.y * 4, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
						{
							if (imageStagingBuffer.bufferData(data, false) &&
								imageStagingBuffer.copyToImage(image, _dimensions))
							{

								result = true;
							}
							else
							{
								imageMemory.destroy();
								vkDestroyImage(LOGICAL_DEVICE_DEVICE, image, nullptr);
							}

							imageStagingBuffer.destroy();
						}
						else
						{
							imageMemory.destroy();
							vkDestroyImage(LOGICAL_DEVICE_DEVICE, image, nullptr);
						}
					}
					else
					{
						if (_format == supportedDepthFormats[0])
						{
							result = transitionLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
						}
						else
						{
							imageMemory.destroy();
							vkDestroyImage(LOGICAL_DEVICE_DEVICE, image, nullptr);
						}
					}
				}
				else
				{
					vkDestroyImage(LOGICAL_DEVICE_DEVICE, image, nullptr);
				}
			}

			return result;
		}

		bool VulkanImage::transitionLayout(VkImageLayout newLayout)
		{
			bool result = false;

			auto layoutAccess = layoutToAccessMap.find(layout);
			auto newLayoutAccess = layoutToAccessMap.find(newLayout);
			auto layoutPipelineStage = layoutToPipelineStageMap.find(layout);
			auto newLayoutPipelineStage = layoutToPipelineStageMap.find(newLayout);

			if (layoutAccess == layoutToAccessMap.end() ||
				newLayoutAccess == layoutToAccessMap.end() ||
				layoutPipelineStage == layoutToPipelineStageMap.end() ||
				newLayoutPipelineStage == layoutToPipelineStageMap.end())
			{
				return result;
			}

			VulkanCommandPool transitionCommandPool;
			if (transitionCommandPool.create())
			{
				if (transitionCommandPool.allocateBuffers(1) && 
					transitionCommandPool.beginBuffers())
				{
					VkCommandBuffer transitionCommand = transitionCommandPool.getNewCommandBuffer(0);

					VkImageMemoryBarrier barrier{};
					barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
					barrier.oldLayout = layout;
					barrier.newLayout = newLayout;
					barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					barrier.image = image;
					barrier.subresourceRange.aspectMask = format == VK_FORMAT_B8G8R8A8_SRGB ? VK_IMAGE_ASPECT_COLOR_BIT : VK_IMAGE_ASPECT_DEPTH_BIT;
					barrier.subresourceRange.baseMipLevel = 0;
					barrier.subresourceRange.levelCount = 1;
					barrier.subresourceRange.baseArrayLayer = 0;
					barrier.subresourceRange.layerCount = 1;

					barrier.srcAccessMask = layoutAccess->second;
					barrier.dstAccessMask = newLayoutAccess->second;

					vkCmdPipelineBarrier
					(
						transitionCommand,
						layoutPipelineStage->second, newLayoutPipelineStage->second,
						0,
						0, nullptr,
						0, nullptr,
						1, &barrier
					);

					if (transitionCommandPool.endBuffers()
						&& transitionCommandPool.execute())
					{
						layout = newLayout;
						result = true;
					}
				}

				transitionCommandPool.destroy();
			}

			return result;
		}

		void VulkanImage::destroy()
		{
			imageMemory.destroy();
			vkDestroyImage(LOGICAL_DEVICE_DEVICE, image, nullptr);
		}

		VkImage VulkanImage::getImage()
		{
			return image;
		}

		u32 VulkanImage::getWidth()
		{
			return dimensions.x;
		}

		u32 VulkanImage::getHeight()
		{
			return dimensions.y;
		}

		glm::uvec2 VulkanImage::getDimensions()
		{
			return dimensions;
		}
	}
}