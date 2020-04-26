#include "Vulkan.h"

#include "VulkanInstance.h"
#include "VulkanDeviceManager.h"
#include "VulkanValidationLayers.h"
#include "VulkanSwapChain.h"

namespace SSE::Vulkan
{
	bool initVulkan(Window& window, VulkanSurface& surface)
	{
		VulkanInstance::gInstance.create(window);

		ValidationLayers::initDebugExtensions();

#pragma message("TODO: ResourceManager to allocate the surface")
		surface.create(window);

		VulkanDeviceManager::gDeviceManager.initDevices(surface);

		return true;
	}

	void cleanupVulkan()
	{
		VulkanDeviceManager::gDeviceManager.cleanupDevices();
		VulkanInstance::gInstance.destroy();
	}
}