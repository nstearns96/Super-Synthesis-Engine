#ifndef _SSE_VULKAN_PHYSICAL_DEVICE_H
#define _SSE_VULKAN_PHYSICAL_DEVICE_H

#include <vulkan/vulkan.h>

#include <vector>

#include "VulkanSurface.h"
#include "VulkanSwapChain.h"

namespace SSE::Vulkan
{
	class VulkanPhysicalDevice
	{
	private:
		VkPhysicalDevice device;

		std::vector<VkQueueFamilyProperties> queueFamilies;
		VkQueueFamilyProperties* graphicsFamily = nullptr;
		VkQueueFamilyProperties* presentFamily = nullptr;

		bool querySwapChainSupport(VulkanSurface& surface);

	public:
		bool initDevice(VkPhysicalDevice _device);

		bool isSuitable(VulkanSurface& surface);

		unsigned int getGraphicsFamilyIndex();
		unsigned int getPresentFamilyIndex();

		SwapChainSupportDetails getSwapChainSupport(VulkanSurface& surface);

		VkPhysicalDevice& getDevice();
	};
}

#endif