#ifndef _SSE_VULKAN_COMMAND_POOL_H
#define _SSE_VULKAN_COMMAND_POOL_H

#include <vector>

#include <vulkan/vulkan.h>

#include "Vulkan/Graphics/VulkanRenderPass.h"
#include "Vulkan/Graphics/VulkanSwapChain.h"
#include "Vulkan/Graphics/VulkanVertexBuffer.h"

namespace SSE::Vulkan
{
	class VulkanCommandPool
	{
	private:
		VkCommandPool commandPool;
		std::vector<VkCommandBuffer> commandBuffers;

	public:

		bool create(VkCommandPoolCreateFlags flags);

		void destroy();

		void freeBuffers();
		bool allocateBuffers(u32 numBuffers);

		bool beginBuffers();
		bool endBuffers();
		bool execute(bool waitGraphics = true);

		VkCommandBuffer& getNewCommandBuffer(u32 index);
		const VkCommandBuffer* getCommandBuffer(u32 index);
	};
}

#endif