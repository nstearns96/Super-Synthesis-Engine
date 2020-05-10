#ifndef _SSE_VULKAN_LOGICAL_DEVICE_DEVICE_H
#define _SSE_VULKAN_LOGICAL_DEVICE_DEVICE_H

#include <vulkan/vulkan.h>


namespace SSE::Vulkan
{
	class VulkanPhysicalDevice;

	class VulkanLogicalDevice
	{
	private:
		VkDevice device;

		VkQueue graphicsQueue;
		VkQueue presentQueue;

		void createQueues(VulkanPhysicalDevice& physicalDevice);

	public:
		bool initDevice(VulkanPhysicalDevice& physicalDevice);
		void cleanupDevice();

		VkDevice& getDevice();

		bool submit(VkSubmitInfo& submitInfo, VkFence& fence, bool& recreateSwapChain);

		void present(VkPresentInfoKHR& presentInfo);
	};

}

#endif