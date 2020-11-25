#ifndef _SSE_VULKAN_RENDER_PASS_H
#define _SSE_VULKAN_RENDER_PASS_H

#include <vulkan/vulkan.h>

#include "Vulkan/Graphics/VulkanSwapChain.h"

namespace SSE::Vulkan
{
	class VulkanRenderPass
	{
	private:
		VkRenderPass renderPass;

	public:
		bool create(VulkanSwapChain& swapChain);

		void destroy();

		VkRenderPass getRenderPass() const;
	};
}

#endif