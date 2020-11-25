#include "Vulkan/VulkanMain.h"

#include "Vulkan/VulkanInstance.h"

#include "Vulkan/Devices/VulkanDeviceManager.h"

#include "Vulkan/Graphics/VulkanSwapChain.h"

#include "Vulkan/Validation Layers/VulkanValidationLayers.h"

namespace SSE::Vulkan
{
	bool initVulkan()
	{
		ValidationLayers::initDebugExtensions();

		VulkanDeviceManager::gDeviceManager.initDevices();

		return true;
	}

	void cleanupVulkan()
	{
		VulkanDeviceManager::gDeviceManager.cleanupDevices();
		VulkanInstance::gInstance.destroy();
	}
}