#include "Vulkan/VulkanMain.h"

#include "Vulkan/VulkanInstance.h"
#include "Vulkan/Devices/VulkanDeviceManager.h"
#include "Vulkan/Validation Layers/VulkanValidationLayers.h"
#include "Vulkan/Graphics/VulkanSwapChain.h"

namespace SSE::Vulkan
{
	bool initVulkan(Window& window, VulkanSurface& surface)
	{
		VulkanInstance::gInstance.create();

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