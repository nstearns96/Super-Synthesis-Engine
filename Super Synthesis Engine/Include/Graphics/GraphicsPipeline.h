#ifndef _SSE_GRAPHICS_PIPELINE_H
#define _SSE_GRAPHICS_PIPELINE_H

#include <vulkan/vulkan.h>

#include "Vulkan/Graphics/VulkanSurface.h"
#include "Vulkan/Graphics/VulkanRenderPass.h"
#include "Vulkan/VulkanCommandPool.h"
#include "Vulkan/Graphics/VulkanVertexBuffer.h"

namespace SSE::Graphics
{
	static const int MAX_FRAMES_IN_FLIGHT = 2;

	class GraphicsPipeline
	{
	private:
		VkPipelineLayout layout;
		VkPipeline pipeline;
		Vulkan::VulkanRenderPass renderPass;
		Vulkan::VulkanSwapChain swapChain;
		std::vector<VkFramebuffer> frameBuffers;
		Vulkan::VulkanCommandPool commandPool;
		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		std::vector<VkFence> imagesInFlight;
		Vulkan::VulkanVertexBuffer vertexBuffer;

		unsigned int currentFrame;

		bool recreateSwapChain();

	public:
		Vulkan::VulkanSurface surface;

		bool create();

		void destroy();

		bool isFrameBufferDirty = false;

		bool executeRenderPass();
	};
}

#endif