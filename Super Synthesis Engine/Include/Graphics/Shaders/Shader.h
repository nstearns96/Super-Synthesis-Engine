#ifndef _SSE_SHADER_H
#define _SSE_SHADER_H

#include <string>
#include <vector>

#include <vulkan/vulkan.h>

#include "Graphics/Shaders/ShaderModuleType.h"

#include "Vulkan/Shaders/VulkanShaderModule.h"

namespace SSE::Graphics
{
	class Shader
	{
	private:
		std::vector<Vulkan::VulkanShaderModule> shaderModules;
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts;

	public:
		bool create(const std::vector<std::string>& sourceFiles, const std::vector<ShaderModuleType>& moduleTypes);

		void destroy();

		Vulkan::VulkanShaderModule getShaderModule(ShaderModuleType moduleType) const;
		std::vector<VkPipelineShaderStageCreateInfo> getShaderStages() const;

		VertexFormat getInputFormat() const;
		std::vector<VkDescriptorSetLayout> getDescriptorSetLayouts() const;
	};
}

#endif