#ifndef _SSE_GRAPHICS_PIPELINE_H
#define _SSE_GRAPHICS_PIPELINE_H

#include <vulkan/vulkan.h>

#include "Vulkan/Graphics/VulkanSurface.h"
#include "Vulkan/Graphics/VulkanRenderPass.h"
#include "Vulkan/VulkanCommandPool.h"
#include "Vulkan/Graphics/VulkanVertexBuffer.h"
#include "Vulkan/Shaders/Descriptors/VulkanDescriptorPool.h"
#include "Graphics/UniformBuffer.h"

namespace SSE::Graphics
{
	static const int MAX_FRAMES_IN_FLIGHT = 2;

	struct MatricesObject
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

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
		std::vector<UniformBuffer<MatricesObject>> uniformBuffers;
		Vulkan::VulkanDescriptorPool descriptorPool;

		unsigned int currentFrame;

		bool recreateSwapChain();
		bool constructPipeline();
		bool constructCommandBuffers();

		void updateUniformBuffers(unsigned int imageIndex);

	public:
		Vulkan::VulkanSurface surface;

		bool create();

		void destroy();

		bool isFrameBufferDirty = false;

		bool executeRenderPass();
	};
}

#endif