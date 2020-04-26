#ifndef _SSE_VULKAN_SURFACE_H
#define _SSE_VULKAN_SURFACE_H

#include <vulkan/vulkan.h>

#include "Window.h"

namespace SSE::Vulkan
{
	class VulkanSurface
	{
	private:
		VkSurfaceKHR surface;

	public:
		bool create(Window& window);
		void destroy();

		VkSurfaceKHR getSurface();
	};
}
#endif