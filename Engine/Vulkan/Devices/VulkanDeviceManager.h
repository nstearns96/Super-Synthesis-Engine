#ifndef _SSE_VULKAN_PHYSICAL_DEVICE_MANAGER_H
#define _SSE_VULKAN_PHYSICAL_DEVICE_MANAGER_H

#include <vector>

#include "VulkanPhysicalDevice.h"
#include "VulkanLogicalDevice.h"
#include "VulkanSurface.h"

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

		bool initDevices(VulkanSurface& surface);
		void cleanupDevices();

		bool checkDeviceExtensionSupport(VulkanPhysicalDevice physicalDevice);
		bool querySwapChainSupport(VulkanSurface* surface);

		const std::vector<const char*> getRequiredDeviceExtensions();

		VulkanPhysicalDevice& getActivePhysicalDevice();
		VulkanLogicalDevice& getActiveLogicalDevice();
	};
}
#endif