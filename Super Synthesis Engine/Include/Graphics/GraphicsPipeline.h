#ifndef _SSE_GRAPHICS_PIPELINE_H
#define _SSE_GRAPHICS_PIPELINE_H

#include <vulkan/vulkan.h>

#include "EngineTypeDefs.h"

#include "Graphics/Shaders/Shader.h"

#include "Vulkan/VulkanCommandPool.h"

#include "Vulkan/Graphics/VulkanRenderPass.h"
#include "Vulkan/Graphics/VulkanSurface.h"
#include "Vulkan/Graphics/VulkanVertexBuffer.h"

#include "Vulkan/Shaders/Descriptors/VulkanDescriptorPool.h"

namespace SSE::Graphics
{
	class GraphicsPipeline
	{
	private:
		VkPipelineLayout layout;
		VkPipeline pipeline;
		ResourceHandle shaderHandle;

	public:
		bool create(ResourceHandle _shaderHandle, const Vulkan::VulkanSwapChain& swapChain, const Vulkan::VulkanRenderPass& renderPass);
		bool recreate(const Vulkan::VulkanSwapChain& swapChain, const Vulkan::VulkanRenderPass& renderPass);
		void destroy();

		VkPipelineLayout getLayout();
		VkPipeline getPipeline();
	};
}

#endif