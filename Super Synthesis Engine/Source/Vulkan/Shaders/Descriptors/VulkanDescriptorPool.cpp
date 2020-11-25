#include "vulkan/Shaders/Descriptors/VulkanDescriptorPool.h"

#include "Logging/Logger.h"

#include "Vulkan/Devices/VulkanDeviceManager.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Vulkan
	{
		VulkanDescriptorPool gDescriptorPool;

		bool VulkanDescriptorPool::create()
		{
#pragma message("TODO: Generalize creation of descriptor pool.")
			std::vector<VkDescriptorPoolSize> poolSizes;
			poolSizes.resize(2);
			poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			poolSizes[0].descriptorCount = 1024;
			poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			poolSizes[1].descriptorCount = 1024;

			VkDescriptorPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
			poolInfo.poolSizeCount = (u32)poolSizes.size();
			poolInfo.pPoolSizes = poolSizes.data();
			poolInfo.maxSets = 2 * 1024;

			if (vkCreateDescriptorPool(LOGICAL_DEVICE_DEVICE, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) 
			{
				GLOG_CRITICAL("Failed to create descriptor pool.");
				return false;
			}

			descriptorSets.resize(2*1024);

			return true;
		}

		void VulkanDescriptorPool::destroy()
		{
			vkDestroyDescriptorPool(LOGICAL_DEVICE_DEVICE, descriptorPool, nullptr);
		}

		std::vector<ResourceHandle> VulkanDescriptorPool::allocateDescriptorSets(const std::vector<VkDescriptorSetLayout>& setLayouts)
		{
			std::vector<ResourceHandle> result;
			for (const auto& layout : setLayouts)
			{
				VkDescriptorSetAllocateInfo allocInfo{};
				allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
				allocInfo.descriptorPool = descriptorPool;
				allocInfo.descriptorSetCount = 1;
				allocInfo.pSetLayouts = &layout;

				ResourceHandle newHandle = descriptorHandles.getHandle();

				if (vkAllocateDescriptorSets(LOGICAL_DEVICE_DEVICE, &allocInfo, &descriptorSets[newHandle]) != VK_SUCCESS)
				{
					GLOG_CRITICAL("Failed to create descriptor layout.");
					descriptorHandles.freeHandle(newHandle);
					return result;
				}

				result.push_back(newHandle);
			}

			return result;
		}

		void VulkanDescriptorPool::freeDescriptorSets(const std::vector<ResourceHandle> _descriptorHandles)
		{
			for (const auto& descriptorHandle : _descriptorHandles)
			{
				vkFreeDescriptorSets(LOGICAL_DEVICE_DEVICE, descriptorPool, 1, &descriptorSets[descriptorHandle]);
				descriptorHandles.freeHandle(descriptorHandle);
			}
		}

		VkDescriptorSet& VulkanDescriptorPool::getDescriptorSet(ResourceHandle resourceHandle)
		{
			return descriptorSets[resourceHandle];
		}

		void VulkanDescriptorPool::updateDescriptorSet(ResourceHandle resourceHandle, std::vector<VkWriteDescriptorSet>& writeDescriptors)
		{
			if (!descriptorHandles.isHandleActive(resourceHandle))
			{
				return;
			}

			VkDescriptorSet& destSet = getDescriptorSet(resourceHandle);

			for (auto& writeDescriptor : writeDescriptors)
			{
				writeDescriptor.dstSet = destSet;
			}

			vkUpdateDescriptorSets(LOGICAL_DEVICE_DEVICE, (u32)writeDescriptors.size(), writeDescriptors.data(), 0, nullptr);
		}
	}
}