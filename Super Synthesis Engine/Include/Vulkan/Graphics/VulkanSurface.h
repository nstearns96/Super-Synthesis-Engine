#ifndef _SSE_VULKAN_SURFACE_H
#define _SSE_VULKAN_SURFACE_H

#include "vulkan/vulkan.h"

#include <SDL/SDL.h>

namespace SSE::Vulkan
{
	class VulkanSurface
	{
	private:
		VkSurfaceKHR surface;

	public:
		bool create(SDL_Window* window);
		void destroy();

		VkSurfaceKHR getSurface() const;
	};
}
#endif