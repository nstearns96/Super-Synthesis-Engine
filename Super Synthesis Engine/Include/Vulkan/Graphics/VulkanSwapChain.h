#ifndef _SSE_VULKAN_SWAP_CHAIN_SUPPORT_H
#define _SSE_VULKAN_SWAP_CHAIN_SUPPORT_H

#include "vulkan/vulkan.h"

#include <vector>

#include "Vulkan/Graphics/VulkanDepthBuffer.h"
#include "Vulkan/Graphics/VulkanImageView.h"
#include "Vulkan/Graphics/VulkanSurface.h"

namespace SSE::Vulkan
{
	class VulkanRenderPass;

	struct SwapChainSupportDetails 
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class VulkanSwapChain
	{
	private:
		VkSurfaceFormatKHR swapSurfaceFormat;
		VkPresentModeKHR swapPresentMode;
		VkExtent2D swapExtent;

		VkSwapchainKHR swapChain;
		std::vector<VkImage> images;
		std::vector<VulkanImageView> imageViews;
		VulkanDepthBuffer depthBuffer;

		u32 imageCount;

		bool createImageViews();

		void destroyImageViews();

	public:
		bool create();

		void destroy();

		VkSurfaceFormatKHR getFormat();
		VkPresentModeKHR getPresentMode();
		VkExtent2D getExtent() const;

		VkSwapchainKHR getSwapChain();

		u32 getImagesInFlight();

		bool createFramebuffers(VulkanRenderPass& renderPass, std::vector<VkFramebuffer>& outFramebuffers);
	};
}

#endif
