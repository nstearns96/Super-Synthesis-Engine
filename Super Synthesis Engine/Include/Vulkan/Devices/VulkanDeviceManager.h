#ifndef _SSE_VULKAN_PHYSICAL_DEVICE_MANAGER_H
#define _SSE_VULKAN_PHYSICAL_DEVICE_MANAGER_H

#include <vector>

#include "Vulkan/Devices/VulkanLogicalDevice.h"
#include "Vulkan/Devices/VulkanPhysicalDevice.h"

#include "Vulkan/Graphics/VulkanSurface.h"

#define LOGICAL_DEVICE Vulkan::VulkanDeviceManager::gDeviceManager.getActiveLogicalDevice()
#define LOGICAL_DEVICE_DEVICE LOGICAL_DEVICE.getDevice()
#define PHYSICAL_DEVICE Vulkan::VulkanDeviceManager::gDeviceManager.getActivePhysicalDevice()
#define PHYSICAL_DEVICE_DEVICE PHYSICAL_DEVICE.getDevice()

namespace SSE::Vulkan
{
	class VulkanDeviceManager
	{
	private:
		VulkanDeviceManager();

		VulkanPhysicalDevice activePhysicalDevice;
		VulkanLogicalDevice activeLogicalDevice;

	public:
		static VulkanDeviceManager gDeviceManager;

		bool initDevices();
		void cleanupDevices();

		bool checkDeviceExtensionSupport(VulkanPhysicalDevice physicalDevice);

		const std::vector<const char*> getRequiredDeviceExtensions();

		VulkanPhysicalDevice& getActivePhysicalDevice();
		VulkanLogicalDevice& getActiveLogicalDevice();
	};
}
#endif