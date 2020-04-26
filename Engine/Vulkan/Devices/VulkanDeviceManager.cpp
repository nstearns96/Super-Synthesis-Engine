#include "VulkanDeviceManager.h"

#include <set>

#include "VulkanInstance.h"
#include "Logger.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Vulkan
	{
		static const std::vector<const char*> requiredDeviceExtensions =
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		VulkanDeviceManager::VulkanDeviceManager() {};

		VulkanDeviceManager VulkanDeviceManager::gDeviceManager;

		bool VulkanDeviceManager::initDevices(VulkanSurface& surface)
		{
			VkInstance instance = VulkanInstance::gInstance.getInstance();

			uint32_t deviceCount = 0;
			vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

			if (deviceCount == 0) {
				gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to find GPU with Vulkan support.");
				return false;
			}

			std::vector<VkPhysicalDevice> devices(deviceCount);
			vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

			bool foundDevice = false;
			for (const auto& physicalDevice : devices)
			{
				if (!activePhysicalDevice.initDevice(physicalDevice)) continue;

				if (activePhysicalDevice.isSuitable(surface) && checkDeviceExtensionSupport(activePhysicalDevice))
				{
					foundDevice = true;
					break;
				}
			}

			if (!foundDevice)
			{
				gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to find suitable physical device.");
				return false;
			}

			if (!activeLogicalDevice.initDevice(activePhysicalDevice))
			{
				return false;
			}

			return true;
		}

		bool VulkanDeviceManager::checkDeviceExtensionSupport(VulkanPhysicalDevice physicalDevice)
		{
			uint32_t extensionCount;
			vkEnumerateDeviceExtensionProperties(physicalDevice.getDevice(), nullptr, &extensionCount, nullptr);

			std::vector<VkExtensionProperties> availableExtensions(extensionCount);
			vkEnumerateDeviceExtensionProperties(physicalDevice.getDevice(), nullptr, &extensionCount, availableExtensions.data());

			std::set<std::string> requiredExtensions(requiredDeviceExtensions.begin(), requiredDeviceExtensions.end());

			for (const auto& extension : availableExtensions)
			{
				requiredExtensions.erase(extension.extensionName);
			}

			return requiredExtensions.empty();
		}

		VulkanPhysicalDevice& VulkanDeviceManager::getActivePhysicalDevice()
		{
			return activePhysicalDevice;
		}

		VulkanLogicalDevice& VulkanDeviceManager::getActiveLogicalDevice()
		{
			return activeLogicalDevice;
		}

		void VulkanDeviceManager::cleanupDevices()
		{
			activeLogicalDevice.cleanupDevice();
		}

		const std::vector<const char*> VulkanDeviceManager::getRequiredDeviceExtensions()
		{
			return requiredDeviceExtensions;
		}
	}
}