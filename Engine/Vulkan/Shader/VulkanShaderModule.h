#ifndef _SSE_VULKAN_SHADER_MODULE_H
#define _SSE_VULKAN_SHADER_MODULE_H

#include <vulkan/vulkan.h>

#include <vector>

#include "VulkanShaderModuleType.h"

namespace SSE::Vulkan
{
	class VulkanShaderModule
	{
	private:
		VkShaderModule shaderModule;
		ShaderModuleType moduleType;

	public:
		bool create(const std::vector<char>& code, ShaderModuleType _moduleType);

		ShaderModuleType getModuleType();
		VkShaderModule getModule();

		void destroy();
	};
}

#endif