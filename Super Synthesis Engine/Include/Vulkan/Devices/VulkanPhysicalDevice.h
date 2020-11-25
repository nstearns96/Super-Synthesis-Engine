#ifndef _SSE_VULKAN_PHYSICAL_DEVICE_H
#define _SSE_VULKAN_PHYSICAL_DEVICE_H

#include <vector>

#include <vulkan/vulkan.h>

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

		bool isSuitable();

		u32 getGraphicsFamilyIndex();
		u32 getPresentFamilyIndex();

		SwapChainSupportDetails getSwapChainSupport(const VulkanSurface& surface);

		VkFormatProperties getFormatProperties(VkFormat format);

		VkPhysicalDevice& getDevice();
	};
}

#endif