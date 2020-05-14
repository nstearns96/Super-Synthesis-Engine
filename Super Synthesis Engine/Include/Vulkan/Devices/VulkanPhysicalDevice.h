#ifndef _SSE_VULKAN_PHYSICAL_DEVICE_H
#define _SSE_VULKAN_PHYSICAL_DEVICE_H

#include <vulkan/vulkan.h>

#include <vector>

#include "Vulkan/Graphics/VulkanSurface.h"
#include "Vulkan/Graphics/VulkanSwapChain.h"

namespace SSE::Vulkan
{
	class VulkanPhysicalDevice
	{
	private:
		VkPhysicalDevice device;

#pragma message("TODO: Generalize queue family support")
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