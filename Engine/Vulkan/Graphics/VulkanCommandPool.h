#ifndef _SSE_VULKAN_COMMAND_POOL_H
#define _SSE_VULKAN_COMMAND_POOL_H

#include <vulkan/vulkan.h>
#include <vector>

#include "VulkanSwapChain.h"
#include "VulkanRenderPass.h"

namespace SSE::Vulkan
{
	class VulkanCommandPool
	{
	private:
		VkCommandPool commandPool;
		std::vector<VkCommandBuffer> commandBuffers;

	public:

		bool create(VulkanSwapChain& swapChain, VulkanRenderPass& renderPass, const std::vector<VkFramebuffer>& frameBuffers, VkPipeline& pipeline);

		void destroy();

		const VkCommandBuffer* getCommandBuffer(int index);
	};
}

#endif