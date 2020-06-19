#ifndef _SSE_VULKAN_DESCRIPTOR_POOL_H
#define _SSE_VULKAN_DESCRIPTOR_POOL_H

#include "vulkan/vulkan.h"

#include <vector>

#include "EngineTypeDefs.h"
#include "Vulkan/Memory/VulkanBuffer.h"
#include "Vulkan/Graphics/VulkanImageView.h"

namespace SSE::Vulkan
{
	class VulkanDescriptorPool
	{
	private:
		VkDescriptorPool descriptorPool;
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
		std::vector<VkDescriptorSet> descriptorSets;

	public:
		bool create(u32 count);

		bool allocateDescriptorSets();

		const VkDescriptorSetLayout* getLayouts();
		void updateDescriptorSet(u32 index, VulkanBuffer uniformBuffer, VulkanImageView imageView, VkSampler sampler);

		VkDescriptorSet& getDescriptorSet(u32 index);

		void destroy();
	};
}

#endif