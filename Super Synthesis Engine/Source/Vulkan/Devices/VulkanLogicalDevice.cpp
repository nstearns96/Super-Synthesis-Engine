#include "Vulkan/Devices/VulkanLogicalDevice.h"

#include <set>

#include "Vulkan/Devices/VulkanDeviceManager.h"
#include "Vulkan/Validation Layers/VulkanValidationLayers.h"
#include "Logging/Logger.h"

namespace SSE
{ 
	extern Logger gLogger;

	namespace Vulkan
	{
		bool VulkanLogicalDevice::initDevice(VulkanPhysicalDevice& physicalDevice)
		{
			std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
			std::set<u32> uniqueQueueFamilies = { physicalDevice.getGraphicsFamilyIndex(),	physicalDevice.getPresentFamilyIndex() };

			r32 queuePriority = 1.0f;
			for (auto& queueFamilyIndex : uniqueQueueFamilies)
			{
				VkDeviceQueueCreateInfo queueCreateInfo{};
				queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
				queueCreateInfo.queueCount = 1;
				queueCreateInfo.pQueuePriorities = &queuePriority;

				queueCreateInfos.push_back(queueCreateInfo);
			}

#pragma message("TODO: Generalize optional device features")
			VkPhysicalDeviceFeatures deviceFeatures{};
			deviceFeatures.samplerAnisotropy = VK_TRUE;

			const std::vector<const char*> requiredDeviceExtensions = VulkanDeviceManager::gDeviceManager.getRequiredDeviceExtensions();

			std::vector<const char*> validationLayers = Vulkan::ValidationLayers::getRequiredLayers();

			VkDeviceCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			createInfo.pQueueCreateInfos = queueCreateInfos.data();
			createInfo.queueCreateInfoCount = (u32)queueCreateInfos.size();
			createInfo.pEnabledFeatures = &deviceFeatures;
			createInfo.enabledExtensionCount = (u32)requiredDeviceExtensions.size();
			createInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();
			createInfo.enabledLayerCount = (u32)validationLayers.size();
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

		VkDevice VulkanLogicalDevice::getDevice()
		{
			return device;
		}

		VkResult VulkanLogicalDevice::submit(VkSubmitInfo& submitInfo, VkFence* fence)
		{
			VkResult result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, (fence == nullptr) ? VK_NULL_HANDLE : *fence);
			if (result != VK_SUCCESS)
			{
				gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to submit render command.");
			}

			return result;
		}

		void VulkanLogicalDevice::waitGraphicsIdle()
		{
			vkQueueWaitIdle(graphicsQueue);
		}

		void VulkanLogicalDevice::present(VkPresentInfoKHR& presentInfo)
		{
			vkQueuePresentKHR(presentQueue, &presentInfo);
		}
	}
}
