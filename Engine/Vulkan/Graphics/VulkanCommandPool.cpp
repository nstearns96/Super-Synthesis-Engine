#include "VulkanCommandPool.h"

#include "VulkanDeviceManager.h"
#include "Logger.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Vulkan
	{
		bool VulkanCommandPool::create(VulkanSwapChain& swapChain, VulkanRenderPass& renderPass, const std::vector<VkFramebuffer>& frameBuffers, VkPipeline& pipeline)
		{
			VkCommandPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			poolInfo.queueFamilyIndex = VulkanDeviceManager::gDeviceManager.getActivePhysicalDevice().getGraphicsFamilyIndex();
			poolInfo.flags = 0;

			if (vkCreateCommandPool(VulkanDeviceManager::gDeviceManager.getActiveLogicalDevice().getDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS) 
			{
				gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to create command pool.");
				return false;
			}

			commandBuffers.resize(frameBuffers.size());

			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.commandPool = commandPool;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

			if (vkAllocateCommandBuffers(VulkanDeviceManager::gDeviceManager.getActiveLogicalDevice().getDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) 
			{
				gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to allocate command buffers.");
				return false;
			}

			for (size_t i = 0; i < commandBuffers.size(); i++) 
			{
				VkCommandBufferBeginInfo beginInfo{};
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				beginInfo.flags = 0;
				beginInfo.pInheritanceInfo = nullptr;

				if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) 
				{
					gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to begin command buffer.");
					return false;
				}

				VkRenderPassBeginInfo renderPassInfo{};
				renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				renderPassInfo.renderPass = renderPass.getRenderPass();
				renderPassInfo.framebuffer = frameBuffers[i];
				renderPassInfo.renderArea.offset = { 0, 0 };
				renderPassInfo.renderArea.extent = swapChain.getExtent();

				VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
				renderPassInfo.clearValueCount = 1;
				renderPassInfo.pClearValues = &clearColor;

				vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
				vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

				vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

				vkCmdEndRenderPass(commandBuffers[i]);

				if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) 
				{
					gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to end command buffer.");
					return false;
				}
			}

			return true;
		}

		void VulkanCommandPool::destroy()
		{
			vkDestroyCommandPool(VulkanDeviceManager::gDeviceManager.getActiveLogicalDevice().getDevice(), commandPool, nullptr);
		}

		const VkCommandBuffer* VulkanCommandPool::getCommandBuffer(int index)
		{
			return &commandBuffers[index % commandBuffers.size()];
		}
	}
}