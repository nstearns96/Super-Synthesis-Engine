#include "Vulkan/Graphics/VulkanSwapChain.h"

#include "Vulkan/Devices/VulkanDeviceManager.h"
#include "Logging/Logger.h"

#include "Vulkan/Graphics/VulkanRenderPass.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Vulkan
	{
		bool VulkanSwapChain::create(VulkanSurface& surface, const glm::uvec2& dimensions)
		{
			if (dimensions.x == 0 || dimensions.y == 0)
			{
				GLOG_CRITICAL("Swap chain dimension was 0. Swap chain was not created.");
				return false;
			}

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

				if (swapExtent.width == 0 || swapExtent.height == 0)
				{
					GLOG_WARNING("Swap chain dimension was 0");
				}
			}
			else 
			{
				VkExtent2D actualExtent = { dimensions.x, dimensions.y};
				
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
			u32 families[] = { activePhysicalDevice.getGraphicsFamilyIndex(), activePhysicalDevice.getPresentFamilyIndex() };
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

			VulkanLogicalDevice activeLogicalDevice = LOGICAL_DEVICE;
			if (vkCreateSwapchainKHR(activeLogicalDevice.getDevice(), &createInfo, nullptr, &swapChain) != VK_SUCCESS)
			{
				GLOG_CRITICAL("Failed to create swapchain.");
				return false;
			}

			vkGetSwapchainImagesKHR(activeLogicalDevice.getDevice(), swapChain, &imageCount, nullptr);
			images.resize(imageCount);
			vkGetSwapchainImagesKHR(activeLogicalDevice.getDevice(), swapChain, &imageCount, images.data());

			createImageViews();

			depthBuffer.create(dimensions);

			return true;
		}

		void VulkanSwapChain::destroy()
		{
			destroyImageViews();
			depthBuffer.destroy();

			vkDestroySwapchainKHR(LOGICAL_DEVICE_DEVICE, swapChain, nullptr);
			images.resize(0);
		}

		bool VulkanSwapChain::createImageViews()
		{
			imageViews.resize(images.size());

			for (st i = 0; i < imageViews.size(); ++i)
			{
				imageViews[i].create(images[i], VK_FORMAT_B8G8R8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
			}

			return true;
		}

		void VulkanSwapChain::destroyImageViews()
		{
			for (auto imageView : imageViews)
			{
				imageView.destroy();
			}

			imageViews.resize(0);
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

			for (st i = 0; i < imageViews.size(); i++)
			{
				std::vector<VkImageView> attachments = 
				{
					imageViews[i].getImageView(),
					depthBuffer.getImageView().getImageView()
				};

				VkFramebufferCreateInfo framebufferInfo{};
				framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				framebufferInfo.renderPass = renderPass.getRenderPass();
				framebufferInfo.attachmentCount = (u32)attachments.size();
				framebufferInfo.pAttachments = attachments.data();
				framebufferInfo.width = swapExtent.width;
				framebufferInfo.height = swapExtent.height;
				framebufferInfo.layers = 1;

				if (vkCreateFramebuffer(LOGICAL_DEVICE_DEVICE, &framebufferInfo, nullptr, &outFramebuffers[i]) != VK_SUCCESS)
				{
					GLOG_CRITICAL("Failed to create framebuffer.");
					return false;
				}
			}

			return true;
		}

		VkSwapchainKHR VulkanSwapChain::getSwapChain()
		{
			return swapChain;
		}

		u32 VulkanSwapChain::getImagesInFlight()
		{
			return (u32)images.size();
		}
	}
}