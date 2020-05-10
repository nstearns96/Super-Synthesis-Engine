#include "Vulkan/VulkanCommandPool.h"

#include "Vulkan/Devices/VulkanDeviceManager.h"
#include "Logging/Logger.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Vulkan
	{
		bool VulkanCommandPool::create(VulkanSwapChain& swapChain, VulkanRenderPass& renderPass, const std::vector<VkFramebuffer>& frameBuffers, VkPipeline& pipeline, VulkanVertexBuffer& vertexBuffer)
		{
			VkCommandPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			poolInfo.queueFamilyIndex = VulkanDeviceManager::gDeviceManager.getActivePhysicalDevice().getGraphicsFamilyIndex();
			poolInfo.flags = 0;

			if (vkCreateCommandPool(LOGICAL_DEVICE_DEVICE, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) 
			{
				gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to create command pool.");
				return false;
			}

			allocateBuffers(swapChain, renderPass, frameBuffers, pipeline, vertexBuffer);

			return true;
		}

		void VulkanCommandPool::destroy()
		{
			freeBuffers();

			vkDestroyCommandPool(LOGICAL_DEVICE_DEVICE, commandPool, nullptr);
		}

		void VulkanCommandPool::freeBuffers()
		{
			vkFreeCommandBuffers(LOGICAL_DEVICE_DEVICE, commandPool, commandBuffers.size(), commandBuffers.data());
		}

		bool VulkanCommandPool::allocateBuffers(VulkanSwapChain& swapChain, VulkanRenderPass& renderPass, const std::vector<VkFramebuffer>& frameBuffers, VkPipeline& pipeline, VulkanVertexBuffer& vertexBuffer)
		{
			commandBuffers.resize(frameBuffers.size());

			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.commandPool = commandPool;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

			if (vkAllocateCommandBuffers(LOGICAL_DEVICE_DEVICE, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
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

				VkBuffer vertexBuffers[] = { vertexBuffer.getVertexBuffer() };
				VkDeviceSize offsets[] = { 0 };
				vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);

				vkCmdDraw(commandBuffers[i], vertexBuffer.getVertexCount(), 1, 0, 0);

				vkCmdEndRenderPass(commandBuffers[i]);

				if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
				{
					gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to end command buffer.");
					return false;
				}
			}

			return true;
		}

		const VkCommandBuffer* VulkanCommandPool::getCommandBuffer(int index)
		{
			return &commandBuffers[index % commandBuffers.size()];
		}
	}
}