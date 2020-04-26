#include "VulkanSwapChain.h"

#include "VulkanDeviceManager.h"
#include "Logger.h"

#include "VulkanRenderPass.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Vulkan
	{
		bool VulkanSwapChain::create(VulkanSurface& surface)
		{
			details = VulkanDeviceManager::gDeviceManager.getActivePhysicalDevice().getSwapChainSupport(surface);

			bool foundFormat = false;
			for (const auto& availableFormat : details.formats)
			{
#pragma message("TODO: Add functionality for calling a function to get required surface formats")
				if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
				{
					swapSurfaceFormat = availableFormat;
					foundFormat = true;
					break;
				}
			}

			if (!foundFormat)
			{
				swapSurfaceFormat = details.formats[0];
			}

			bool foundPresentMode = false;
			for (const auto& availablePresentMode : details.presentModes) 
			{
				if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) 
				{
					swapPresentMode = availablePresentMode;
					foundPresentMode = true;
					break;
				}
			}

			if (!foundPresentMode)
			{
				swapPresentMode = VK_PRESENT_MODE_FIFO_KHR;
			}

			if (details.capabilities.currentExtent.width != UINT32_MAX) 
			{
				swapExtent = details.capabilities.currentExtent;
			}
			else 
			{
#pragma message("TODO: Query main window for extent")
				VkExtent2D actualExtent = { 1920,1080 };

				actualExtent.width = SDL_max(details.capabilities.minImageExtent.width, SDL_min(details.capabilities.maxImageExtent.width, actualExtent.width));
				actualExtent.height = SDL_max(details.capabilities.minImageExtent.height, SDL_min(details.capabilities.maxImageExtent.height, actualExtent.height));

				swapExtent = actualExtent;
			}

			if (details.capabilities.minImageCount < details.capabilities.maxImageCount)
			{
				imageCount = details.capabilities.minImageCount + 1;
			}
			else
			{
				imageCount = details.capabilities.minImageCount;
			}

			VkSwapchainCreateInfoKHR createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			createInfo.surface = surface.getSurface();
			createInfo.minImageCount = imageCount;
			createInfo.imageFormat = swapSurfaceFormat.format;
			createInfo.imageColorSpace = swapSurfaceFormat.colorSpace;
			createInfo.imageExtent = swapExtent;
#pragma message("TODO: Support for stereoscopic/VR swap chain images")
			createInfo.imageArrayLayers = 1;
#pragma message("TODO: Support for render-to-texture with VK_IMAGE_USAGE_TRANSFER_DST_BIT")
			createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			
			VulkanPhysicalDevice activePhysicalDevice = VulkanDeviceManager::gDeviceManager.getActivePhysicalDevice();
			unsigned int families[] = { activePhysicalDevice.getGraphicsFamilyIndex(), activePhysicalDevice.getPresentFamilyIndex() };
			if (families[0] == families[1])
			{
				createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
				createInfo.queueFamilyIndexCount = 0;
				createInfo.pQueueFamilyIndices = nullptr;
			}
			else
			{
				createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
				createInfo.queueFamilyIndexCount = 2;
				createInfo.pQueueFamilyIndices = families;
			}

			createInfo.preTransform = details.capabilities.currentTransform;
			createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			createInfo.presentMode = swapPresentMode;
			createInfo.clipped = VK_TRUE;
			createInfo.oldSwapchain = VK_NULL_HANDLE;

			VulkanLogicalDevice activeLogicalDevice = VulkanDeviceManager::gDeviceManager.getActiveLogicalDevice();
			if (vkCreateSwapchainKHR(activeLogicalDevice.getDevice(), &createInfo, nullptr, &swapChain) != VK_SUCCESS)
			{
				gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to create swapchain.");
				return false;
			}

			vkGetSwapchainImagesKHR(activeLogicalDevice.getDevice(), swapChain, &imageCount, nullptr);
			images.resize(imageCount);
			vkGetSwapchainImagesKHR(activeLogicalDevice.getDevice(), swapChain, &imageCount, images.data());

			createImageViews();

			return true;
		}

		void VulkanSwapChain::destroy()
		{
			destroyImageViews();

			vkDestroySwapchainKHR(VulkanDeviceManager::gDeviceManager.getActiveLogicalDevice().getDevice(), swapChain, nullptr);
		}

		bool VulkanSwapChain::createImageViews()
		{
			VulkanLogicalDevice activeLogicalDevice = VulkanDeviceManager::gDeviceManager.getActiveLogicalDevice();

			imageViews.resize(images.size());

			for (int i = 0; i < imageViews.size(); ++i)
			{
				VkImageViewCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				createInfo.image = images[i];
				createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				createInfo.format = swapSurfaceFormat.format;
				createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
				createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
				createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
				createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
				createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				createInfo.subresourceRange.baseMipLevel = 0;
				createInfo.subresourceRange.levelCount = 1;
				createInfo.subresourceRange.baseArrayLayer = 0;
				createInfo.subresourceRange.layerCount = 1;

				if (vkCreateImageView(activeLogicalDevice.getDevice(), &createInfo, nullptr, &imageViews[i]) != VK_SUCCESS)
				{
					gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to create image view for swap chain.");
					return false;
				}
			}

			return true;
		}

		void VulkanSwapChain::destroyImageViews()
		{
			for (auto imageView : imageViews)
			{
				vkDestroyImageView(VulkanDeviceManager::gDeviceManager.getActiveLogicalDevice().getDevice(), imageView, nullptr);
			}
		}

		VkSurfaceFormatKHR VulkanSwapChain::getFormat()
		{
			return swapSurfaceFormat;
		}

		VkExtent2D VulkanSwapChain::getExtent()
		{
			return swapExtent;
		}

		VkPresentModeKHR VulkanSwapChain::getPresentMode()
		{
			return swapPresentMode;
		}

		bool VulkanSwapChain::createFramebuffers(VulkanRenderPass& renderPass, std::vector<VkFramebuffer>& outFramebuffers)
		{
			outFramebuffers.resize(imageViews.size());

			for (size_t i = 0; i < imageViews.size(); i++) {
				VkImageView attachments[] = {
					imageViews[i]
				};

				VkFramebufferCreateInfo framebufferInfo{};
				framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				framebufferInfo.renderPass = renderPass.getRenderPass();
				framebufferInfo.attachmentCount = 1;
				framebufferInfo.pAttachments = attachments;
				framebufferInfo.width = swapExtent.width;
				framebufferInfo.height = swapExtent.height;
				framebufferInfo.layers = 1;

				if (vkCreateFramebuffer(VulkanDeviceManager::gDeviceManager.getActiveLogicalDevice().getDevice(), &framebufferInfo, nullptr, &outFramebuffers[i]) != VK_SUCCESS)
				{
					gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to create framebuffer.");
					return false;
				}
			}

			return true;
		}

		VkSwapchainKHR VulkanSwapChain::getSwapChain()
		{
			return swapChain;
		}
	}
}