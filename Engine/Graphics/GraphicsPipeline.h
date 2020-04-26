#ifndef _SSE_GRAPHICS_PIPELINE_H
#define _SSE_GRAPHICS_PIPELINE_H

#include <vulkan/vulkan.h>

#include "VulkanSurface.h"
#include "VulkanRenderPass.h"
#include "VulkanCommandPool.h"

namespace SSE::Graphics
{
	class GraphicsPipeline
	{
	private:
		VkPipelineLayout layout;
		VkPipeline pipeline;
		Vulkan::VulkanRenderPass renderPass;
		Vulkan::VulkanSwapChain swapChain;
		std::vector<VkFramebuffer> frameBuffers;
		Vulkan::VulkanCommandPool commandPool;
		VkSemaphore imageAvailableSemaphore;
		VkSemaphore renderFinishedSemaphore;

	public:
		bool create(Vulkan::VulkanSurface& surface);

		void destroy();

		bool executeRenderPass();
	};
}

#endif