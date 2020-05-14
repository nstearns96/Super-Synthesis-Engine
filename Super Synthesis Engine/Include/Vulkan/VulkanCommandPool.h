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

		bool create();

		void destroy();

		void freeBuffers();
		bool allocateBuffers(unsigned int numBuffers);

		VkCommandBuffer& getNewCommandBuffer(unsigned int index);
		const VkCommandBuffer* getCommandBuffer(unsigned int index);
	};
}

#endif