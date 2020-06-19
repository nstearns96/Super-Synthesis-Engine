#include "vulkan/Shaders/Descriptors/VulkanDescriptorPool.h"

#include "Logging/Logger.h"
#include "vulkan/Devices/VulkanDeviceManager.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Vulkan
	{
		bool VulkanDescriptorPool::create(u32 count)
		{
			std::vector<VkDescriptorPoolSize> poolSizes;
			poolSizes.resize(2);
			poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			poolSizes[0].descriptorCount = count;
			poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			poolSizes[1].descriptorCount = count;

			VkDescriptorPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolInfo.poolSizeCount = (u32)poolSizes.size();
			poolInfo.pPoolSizes = poolSizes.data();
			poolInfo.maxSets = count;

			if (vkCreateDescriptorPool(LOGICAL_DEVICE_DEVICE, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) 
			{
				gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to create descriptor pool.");
				return false;
			}
			
			descriptorSetLayouts.resize(count);
			for (st d = 0; d < descriptorSetLayouts.size(); ++d)
			{
#pragma message("TODO: Contruct descriptor bindings dynamiccally from shader metadata")
				VkDescriptorSetLayoutBinding layoutBinding{};
				layoutBinding.binding = 0;
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				layoutBinding.descriptorCount = 1;
				layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
				layoutBinding.pImmutableSamplers = nullptr;

				VkDescriptorSetLayoutBinding samplerLayoutBinding{};
				samplerLayoutBinding.binding = 1;
				samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				samplerLayoutBinding.descriptorCount = 1;
				samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				samplerLayoutBinding.pImmutableSamplers = nullptr;

				std::vector<VkDescriptorSetLayoutBinding> layoutBindings = { layoutBinding, samplerLayoutBinding };

				VkDescriptorSetLayoutCreateInfo layoutInfo{};
				layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				layoutInfo.bindingCount = (u32)layoutBindings.size();
				layoutInfo.pBindings = layoutBindings.data();

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
			for (st d = 0; d < descriptorSetLayouts.size(); ++d)
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
			allocInfo.descriptorSetCount = (u32)descriptorSetLayouts.size();
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

		VkDescriptorSet& VulkanDescriptorPool::getDescriptorSet(u32 index)
		{
			return descriptorSets[index % descriptorSets.size()];
		}

		void VulkanDescriptorPool::updateDescriptorSet(u32 index, VulkanBuffer uniformBuffer, VulkanImageView imageView, VkSampler sampler)
		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uniformBuffer.getBuffer();
			bufferInfo.offset = 0;
			bufferInfo.range = uniformBuffer.getSize();

			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = imageView.getImageView();
			imageInfo.sampler = sampler;

			std::vector<VkWriteDescriptorSet> descriptorWrites;
			descriptorWrites.resize(2);

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = descriptorSets[index];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = descriptorSets[index];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(LOGICAL_DEVICE_DEVICE, (u32)descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
		}
	}
}