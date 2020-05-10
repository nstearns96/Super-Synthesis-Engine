#ifndef _SSE_VULKAN_INSTANCE_H
#define _SSE_VULKAN_INSTANCE_H

#include <vulkan/vulkan.h>

#include "Window/Window.h"

namespace SSE::Vulkan
{
	class VulkanInstance
	{
	private:
		VkInstance instance;
	
		VulkanInstance();
	public:
		static VulkanInstance gInstance;

		bool create(Window& window);
		void destroy();

		VkInstance getInstance();
	};
}

#endif