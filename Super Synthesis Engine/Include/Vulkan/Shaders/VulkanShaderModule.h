#ifndef _SSE_VULKAN_SHADER_MODULE_H
#define _SSE_VULKAN_SHADER_MODULE_H

#include <vector>

#include <vulkan/vulkan.h>

#include "EngineTypeDefs.h"
#include "VulkanShaderModuleType.h"

namespace SSE::Vulkan
{
	class VulkanShaderModule
	{
	private:
		VkShaderModule shaderModule;
		ShaderModuleType moduleType;

	public:
		bool create(const std::vector<byte>& code, ShaderModuleType _moduleType);

		void destroy();

		ShaderModuleType getModuleType();
		VkShaderModule getModule();

	};
}

#endif