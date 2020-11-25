#ifndef _SSE_VULKAN_MAIN_H
#define _SSE_VULKAN_MAIN_H

#include "Window/Window.h"

#include "Vulkan/Graphics/VulkanSurface.h"

namespace SSE::Vulkan
{
	bool initVulkan();

	void cleanupVulkan();
}

#endif