#include "Vulkan/Graphics/VulkanSurface.h"

#include <SDL/SDL.h>
#include <SDL/SDL_vulkan.h>

#include "Logging/Logger.h"

#include "Vulkan/VulkanInstance.h"

namespace SSE
{
	extern Logger gLogger;
}

namespace SSE::Vulkan
{
	bool VulkanSurface::create(SDL_Window* window)
	{
		if (SDL_Vulkan_CreateSurface(window, VulkanInstance::gInstance.getInstance(), &surface) != SDL_TRUE)
		{
			GLOG_CRITICAL(SDL_GetError());
			return false;
		}
		return true;
	}

	void VulkanSurface::destroy()
	{
		vkDestroySurfaceKHR(VulkanInstance::gInstance.getInstance(),surface, nullptr);
	}

	VkSurfaceKHR VulkanSurface::getSurface() const
	{
		return surface;
	}
}
