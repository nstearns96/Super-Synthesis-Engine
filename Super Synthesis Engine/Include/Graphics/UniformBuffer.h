#ifndef _SSE_UNIFORM_BUFFER_H
#define _SSE_UNIFORM_BUFFER_H

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include "Vulkan/VulkanBuffer.h"
#include "Vulkan/Devices/VulkanDeviceManager.h"

namespace SSE::Graphics
{
	template<class T>
	class UniformBuffer
	{
	private:
		Vulkan::VulkanBuffer uniformBuffer;

	public:

		bool create()
		{
			return uniformBuffer.create(sizeof(T), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		}

		void destroy()
		{
			uniformBuffer.destroy();
		}

		bool updateBuffer(const T& object)
		{
			return uniformBuffer.bufferData((void *)&object, false);
		}

		void updateDescriptorSet(VkDescriptorSet descriptorSet)
		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uniformBuffer.getBuffer();
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(T);

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = descriptorSet;
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;

			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;

			descriptorWrite.pBufferInfo = &bufferInfo;
			descriptorWrite.pImageInfo = nullptr;
			descriptorWrite.pTexelBufferView = nullptr;

			vkUpdateDescriptorSets(LOGICAL_DEVICE_DEVICE, 1, &descriptorWrite, 0, nullptr);
		}
	};
}

#endif