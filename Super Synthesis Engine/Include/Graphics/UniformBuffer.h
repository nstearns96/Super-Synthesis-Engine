#ifndef _SSE_UNIFORM_BUFFER_H
#define _SSE_UNIFORM_BUFFER_H

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include "Vulkan/Memory/VulkanBuffer.h"
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

		Vulkan::VulkanBuffer getBuffer()
		{
			return uniformBuffer;
		}

		bool updateBuffer(const T& object)
		{
			return uniformBuffer.bufferData((void *)&object, false);
		}
	};
}

#endif