#ifndef _SSE_VULKAN_DESCRIPTOR_POOL_H
#define _SSE_VULKAN_DESCRIPTOR_POOL_H

#include "vulkan/vulkan.h"

#include <vector>

namespace SSE::Vulkan
{
	class VulkanDescriptorPool
	{
	private:
		VkDescriptorPool descriptorPool;
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
		std::vector<VkDescriptorSet> descriptorSets;

	public:
		bool create(unsigned int count);

		bool allocateDescriptorSets();

		const VkDescriptorSetLayout* getLayouts();

		VkDescriptorSet& getDescriptorSet(unsigned int index);

		void destroy();
	};
}

#endif