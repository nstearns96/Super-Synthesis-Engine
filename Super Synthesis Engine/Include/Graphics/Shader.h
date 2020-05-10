#ifndef _SSE_SHADER_H
#define _SSE_SHADER_H

#include <vulkan/vulkan.h>

#include <vector>
#include <string>

#include "Vulkan/Shaders/VulkanShaderModule.h"

namespace SSE::Graphics
{
	class Shader
	{
	private:
		std::vector<Vulkan::VulkanShaderModule> shaderModules;
		//Shader stage stuff here

	public:
		bool create(const std::vector<std::string>& sourceFiles, const std::vector<SSE::Vulkan::ShaderModuleType>& moduleTypes);

		void destroy();

		Vulkan::VulkanShaderModule getShaderModule(Vulkan::ShaderModuleType moduleType);
	};
}

#endif