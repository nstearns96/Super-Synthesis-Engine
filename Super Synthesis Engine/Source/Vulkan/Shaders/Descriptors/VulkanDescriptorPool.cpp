#include "vulkan/Shaders/Descriptors/VulkanDescriptorPool.h"

#include "Logging/Logger.h"
#include "vulkan/Devices/VulkanDeviceManager.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Vulkan
	{
		bool VulkanDescriptorPool::create(unsigned int count)
		{
			VkDescriptorPoolSize poolSize{};
			poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			poolSize.descriptorCount = count;

			VkDescriptorPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolInfo.poolSizeCount = 1;
			poolInfo.pPoolSizes = &poolSize;
			poolInfo.maxSets = count;

			if (vkCreateDescriptorPool(LOGICAL_DEVICE_DEVICE, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) 
			{
				gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to create descriptor pool.");
				return false;
			}
			
			descriptorSetLayouts.resize(count);
			for (int d = 0; d < descriptorSetLayouts.size(); ++d)
			{
#pragma message("TODO: Contruct descriptor bindings dynamiccally from shader metadata")
				VkDescriptorSetLayoutBinding layoutBinding{};
				layoutBinding.binding = 0;
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				layoutBinding.descriptorCount = 1;
				layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
				layoutBinding.pImmutableSamplers = nullptr;

				VkDescriptorSetLayoutCreateInfo layoutInfo{};
				layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				layoutInfo.bindingCount = 1;
				layoutInfo.pBindings = &layoutBinding;

				if (vkCreateDescriptorSetLayout(LOGICAL_DEVICE_DEVICE, &layoutInfo, nullptr, &descriptorSetLayouts[d]) != VK_SUCCESS) 
				{
					gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to create descriptor layout.");
					return false;
				}
			}

			descriptorSets.resize(count);

			return allocateDescriptorSets();
		}

		void VulkanDescriptorPool::destroy()
		{
			for (int d = 0; d < descriptorSetLayouts.size(); ++d)
			{
				vkDestroyDescriptorSetLayout(LOGICAL_DEVICE_DEVICE, descriptorSetLayouts[d], nullptr);
			}
			vkDestroyDescriptorPool(LOGICAL_DEVICE_DEVICE, descriptorPool, nullptr);
		}

		bool VulkanDescriptorPool::allocateDescriptorSets()
		{
			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = descriptorPool;
			allocInfo.descriptorSetCount = descriptorSetLayouts.size();
			allocInfo.pSetLayouts = descriptorSetLayouts.data();

			if (vkAllocateDescriptorSets(LOGICAL_DEVICE_DEVICE, &allocInfo, descriptorSets.data()) != VK_SUCCESS) 
			{
				gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to create descriptor layout.");
				return false;
			}

			return true;
		}

		const VkDescriptorSetLayout* VulkanDescriptorPool::getLayouts()
		{
			return descriptorSetLayouts.data();
		}

		VkDescriptorSet& VulkanDescriptorPool::getDescriptorSet(unsigned int index)
		{
			return descriptorSets[index % descriptorSets.size()];
		}

	}
}