#ifndef _SSE_VULKAN_VALIDATION_LAYERS_H
#define _SSE_VULKAN_VALIDATION_LAYERS_H

#include <SDL/SDL_vulkan.h>

#include <vector>
#include <vulkan/vulkan.h>
#include <cstring>
#include <iostream>

#include "Window/Window.h"
#include "Vulkan/VulkanInstance.h"

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