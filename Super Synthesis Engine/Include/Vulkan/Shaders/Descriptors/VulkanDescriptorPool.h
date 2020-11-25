#ifndef _SSE_VULKAN_DESCRIPTOR_POOL_H
#define _SSE_VULKAN_DESCRIPTOR_POOL_H

#include <vector>

#include <vulkan/vulkan.h>

#include "EngineTypeDefs.h"

#include "Resources/ResourceHandleManager.h"

#include "Vulkan/Graphics/VulkanImageView.h"

#include "Vulkan/Memory/VulkanBuffer.h"

namespace SSE::Vulkan
{
	class VulkanDescriptorPool
	{
	private:
		VkDescriptorPool descriptorPool;
#pragma message("TODO: Should probably switch any vectors that use a ResourceHandleManager to a sparse vector")
		std::vector<VkDescriptorSet> descriptorSets;
		ResourceHandleManager descriptorHandles;

	public:
		bool create();

		std::vector<ResourceHandle> allocateDescriptorSets(const std::vector<VkDescriptorSetLayout>& setLayouts);
		void freeDescriptorSets(const std::vector<ResourceHandle> descriptorHandles);

		void updateDescriptorSet(ResourceHandle resourceHandle, std::vector<VkWriteDescriptorSet>& writeDescriptors);

		VkDescriptorSet& getDescriptorSet(ResourceHandle resourceHandle);

		void destroy();
	};

	extern VulkanDescriptorPool gDescriptorPool;
}

#endif