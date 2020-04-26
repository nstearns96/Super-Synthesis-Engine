#include "VulkanLogicalDevice.h"

#include <set>

#include "VulkanDeviceManager.h"
#include "VulkanValidationLayers.h"
#include "Logger.h"

namespace SSE
{ 
	extern Logger gLogger;

	namespace Vulkan
	{
		bool VulkanLogicalDevice::initDevice(VulkanPhysicalDevice& physicalDevice)
		{
			std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
			std::set<unsigned int> uniqueQueueFamilies = { physicalDevice.getGraphicsFamilyIndex(),	physicalDevice.getPresentFamilyIndex() };

			float queuePriority = 1.0f;
			for (auto& queueFamilyIndex : uniqueQueueFamilies)
			{
				VkDeviceQueueCreateInfo queueCreateInfo{};
				queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
				queueCreateInfo.queueCount = 1;
				queueCreateInfo.pQueuePriorities = &queuePriority;

				queueCreateInfos.push_back(queueCreateInfo);
			}

			VkPhysicalDeviceFeatures deviceFeatures{};

			const std::vector<const char*> requiredDeviceExtensions = VulkanDeviceManager::gDeviceManager.getRequiredDeviceExtensions();

			VkDeviceCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

			createInfo.pQueueCreateInfos = queueCreateInfos.data();
			createInfo.queueCreateInfoCount = queueCreateInfos.size();

			createInfo.pEnabledFeatures = &deviceFeatures;

			createInfo.enabledExtensionCount = requiredDeviceExtensions.size();
			createInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();

			std::vector<const char*> validationLayers = Vulkan::ValidationLayers::getRequiredLayers();

			createInfo.enabledLayerCount = validationLayers.size();
			createInfo.ppEnabledLayerNames = validationLayers.data();

			if (vkCreateDevice(physicalDevice.getDevice(), &createInfo, nullptr, &device) != VK_SUCCESS)
			{
				gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to create logical device.");
				return false;
			}

			createQueues(physicalDevice);

			return true;
		}

		void VulkanLogicalDevice::cleanupDevice()
		{
			vkDestroyDevice(device, nullptr);
		}

		void VulkanLogicalDevice::createQueues(VulkanPhysicalDevice& physicalDevice)
		{
			vkGetDeviceQueue(device, physicalDevice.getGraphicsFamilyIndex(), 0, &graphicsQueue);
			vkGetDeviceQueue(device, physicalDevice.getPresentFamilyIndex(), 0, &presentQueue);
		}

		VkDevice& VulkanLogicalDevice::getDevice()
		{
			return device;
		}

		bool VulkanLogicalDevice::submit(VkSubmitInfo& submitInfo)
		{
			if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) 
			{
				gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to submit render command.");
				return false;
			}

			return true;
		}

		void VulkanLogicalDevice::present(VkPresentInfoKHR& presentInfo)
		{
			vkQueuePresentKHR(presentQueue, &presentInfo);

			vkQueueWaitIdle(presentQueue);
		}
	}
}
