#include "Vulkan/Devices/VulkanPhysicalDevice.h"

#include "Logging/Logger.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Vulkan
	{
		bool VulkanPhysicalDevice::isSuitable(VulkanSurface& surface)
		{
			if (graphicsFamily != nullptr && presentFamily != nullptr)
			{
				return true;
			}
			else
			{
				for (u32 i = 0; i < queueFamilies.size(); ++i)
				{
					if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
					{
						graphicsFamily = &queueFamilies[i];
					}

					VkBool32 presentSupport = false;
					vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface.getSurface(), &presentSupport);

					if (presentSupport)
					{
						presentFamily = &queueFamilies[i];
					}

					if (graphicsFamily == presentFamily)
					{
						break;
					}
				}
			}

#pragma message("TODO: Query this from required optional device features")
			VkPhysicalDeviceFeatures supportedFeatures;
			vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

			return graphicsFamily != nullptr && presentFamily != nullptr && querySwapChainSupport(surface) && supportedFeatures.samplerAnisotropy;
		}

		bool VulkanPhysicalDevice::initDevice(VkPhysicalDevice _device)
		{
			device = _device;

			u32 queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

			if (queueFamilyCount == 0) return false;

			queueFamilies.resize(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, &queueFamilies[0]);
			return true;
		}

		u32 VulkanPhysicalDevice::getGraphicsFamilyIndex()
		{
			if (graphicsFamily != nullptr)
			{
				return (u32)(graphicsFamily - &queueFamilies[0]);
			}
			else return 0;
		}

		u32 VulkanPhysicalDevice::getPresentFamilyIndex()
		{
			if (presentFamily != nullptr)
			{
				return (u32)(presentFamily - &queueFamilies[0]);
			}
			else return 0;
		}

		VkPhysicalDevice& VulkanPhysicalDevice::getDevice()
		{
			return device;
		}

		bool VulkanPhysicalDevice::querySwapChainSupport(VulkanSurface& surface)
		{
			SwapChainSupportDetails details;

			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface.getSurface(), &details.capabilities);

			u32 formatCount;
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.getSurface(), &formatCount, nullptr);

			if (formatCount != 0) 
			{
				details.formats.resize(formatCount);
				vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.getSurface(), &formatCount, details.formats.data());
			}
			else
			{
				GLOG_CRITICAL("Couldn't find supported format for surface/device pair.");
				return false;
			}

			u32 presentModeCount;
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface.getSurface(), &presentModeCount, nullptr);

			if (presentModeCount != 0)
			{
				details.presentModes.resize(presentModeCount);
				vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface.getSurface(), &presentModeCount, details.presentModes.data());
			}
			else
			{
				GLOG_CRITICAL("Couldn't find supported present mode for surface/device pair.");
				return false;
			}

			return !details.formats.empty() && !details.presentModes.empty();
		}

		SwapChainSupportDetails VulkanPhysicalDevice::getSwapChainSupport(VulkanSurface& surface)
		{
			SwapChainSupportDetails details;

			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface.getSurface(), &details.capabilities);

			u32 formatCount;
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.getSurface(), &formatCount, nullptr);

			if (formatCount != 0) 
			{
				details.formats.resize(formatCount);
				vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.getSurface(), &formatCount, details.formats.data());
			}
			else
			{
				GLOG_CRITICAL("Couldn't find supported format for surface/device pair.");
				return details;
			}

			u32 presentModeCount;
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface.getSurface(), &presentModeCount, nullptr);

			if (presentModeCount != 0)
			{
				details.presentModes.resize(presentModeCount);
				vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface.getSurface(), &presentModeCount, details.presentModes.data());
			}
			else
			{
				GLOG_CRITICAL("Couldn't find supported present mode for surface/device pair.");
				return details;
			}

			return details;
		}

		VkFormatProperties VulkanPhysicalDevice::getFormatProperties(VkFormat format)
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(device, format, &props);

			return props;
		}
	}
}