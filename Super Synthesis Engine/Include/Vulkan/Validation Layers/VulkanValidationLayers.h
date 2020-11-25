#ifndef _SSE_VULKAN_VALIDATION_LAYERS_H
#define _SSE_VULKAN_VALIDATION_LAYERS_H

#include <cstring>
#include <iostream>
#include <vector>

#include <SDL/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include "Vulkan/VulkanInstance.h"

#include "Window/Window.h"

namespace SSE::Vulkan::ValidationLayers
{
#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

	std::vector<const char*> getRequiredExtensions();
	std::vector<const char*> getRequiredLayers();

	bool checkValidationLayerSupport();

	void getDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	void initDebugExtensions();
	void cleanupDebugExtensions();
}

#endif