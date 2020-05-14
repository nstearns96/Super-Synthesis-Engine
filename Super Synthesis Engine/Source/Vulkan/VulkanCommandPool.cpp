#include "Vulkan/VulkanCommandPool.h"

#include "Vulkan/Devices/VulkanDeviceManager.h"
#include "Logging/Logger.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Vulkan
	{
		bool VulkanCommandPool::create()
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

			commandBuffers.clear();
		}

		bool VulkanCommandPool::allocateBuffers(unsigned int numBuffers)
		{
			commandBuffers.resize(numBuffers);

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

			return true;
		}

		VkCommandBuffer& VulkanCommandPool::getNewCommandBuffer(unsigned int index)
		{
			return commandBuffers[index% commandBuffers.size()];
		}

		const VkCommandBuffer* VulkanCommandPool::getCommandBuffer(unsigned int index)
		{
			return &commandBuffers[index % commandBuffers.size()];
		}
	}
}