#include "Vulkan/VulkanCommandPool.h"

#include "EngineTypeDefs.h"

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
				GLOG_CRITICAL("Failed to create command pool.");
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
			vkFreeCommandBuffers(LOGICAL_DEVICE_DEVICE, commandPool, (u32)commandBuffers.size(), commandBuffers.data());

			commandBuffers.clear();
		}

		bool VulkanCommandPool::allocateBuffers(u32 numBuffers)
		{
			commandBuffers.resize(numBuffers);

			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.commandPool = commandPool;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandBufferCount = (u32)commandBuffers.size();

			if (vkAllocateCommandBuffers(LOGICAL_DEVICE_DEVICE, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
			{
				GLOG_CRITICAL("Failed to allocate command buffers.");
				commandBuffers.clear();
				return false;
			}

			return true;
		}

		bool VulkanCommandPool::beginBuffers()
		{
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			for (st b = 0; b < commandBuffers.size(); ++b)
			{
				if (vkBeginCommandBuffer(commandBuffers[b], &beginInfo) != VK_SUCCESS)
				{
					return false;
				}
			}

			return true;
		}

		bool VulkanCommandPool::endBuffers()
		{
			for (st b = 0; b < commandBuffers.size(); ++b)
			{
				if (vkEndCommandBuffer(commandBuffers[b]) != VK_SUCCESS)
				{
					return false;
				}
			}

			return true;
		}

		bool VulkanCommandPool::execute(bool waitGraphics)
		{
			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = (u32)commandBuffers.size();
			submitInfo.pCommandBuffers = commandBuffers.data();

			VulkanLogicalDevice logicalDevice = LOGICAL_DEVICE;
			if (logicalDevice.submit(submitInfo, nullptr) != VK_SUCCESS)
			{
				return false;
			}

			if (waitGraphics)
			{
				logicalDevice.waitGraphicsIdle();
			}

			return true;
		}

		VkCommandBuffer& VulkanCommandPool::getNewCommandBuffer(u32 index)
		{
			return commandBuffers[index% commandBuffers.size()];
		}

		const VkCommandBuffer* VulkanCommandPool::getCommandBuffer(u32 index)
		{
			return &commandBuffers[index % commandBuffers.size()];
		}
	}
}