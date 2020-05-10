#ifndef _SSE_VULKAN_COMMAND_POOL_H
#define _SSE_VULKAN_COMMAND_POOL_H

#include <vulkan/vulkan.h>
#include <vector>

#include "Vulkan/Graphics/VulkanSwapChain.h"
#include "Vulkan/Graphics/VulkanRenderPass.h"
#include "Vulkan/Graphics/VulkanVertexBuffer.h"

namespace SSE::Vulkan
{
	class VulkanCommandPool
	{
	private:
		VkCommandPool commandPool;
		std::vector<VkCommandBuffer> commandBuffers;

	public:

		bool create(VulkanSwapChain& swapChain, VulkanRenderPass& renderPass, const std::vector<VkFramebuffer>& frameBuffers, VkPipeline& pipeline, VulkanVertexBuffer& vertexBuffer);

		void destroy();

		void freeBuffers();
		bool allocateBuffers(VulkanSwapChain& swapChain, VulkanRenderPass& renderPass, const std::vector<VkFramebuffer>& frameBuffers, VkPipeline& pipeline, VulkanVertexBuffer& vertexBuffer);

		const VkCommandBuffer* getCommandBuffer(int index);
	};
}

#endif