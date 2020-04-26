#ifndef _SSE_VULKAN_H
#define _SSE_VULKAN_H

#include "Window.h"
#include "VulkanSurface.h"

namespace SSE::Vulkan
{
	bool initVulkan(Window& window, VulkanSurface& surface);

	void cleanupVulkan();
}

#endif