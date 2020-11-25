#ifndef _SSE_VULKAN_SHADER_MODULE_H
#define _SSE_VULKAN_SHADER_MODULE_H

#include <vector>

#include <spirv_reflect.h>
#include <vulkan/vulkan.h>

#include "EngineTypeDefs.h"

#include "Graphics/Shaders/ShaderModuleType.h"

#include "Model/VertexFormat.h"

namespace SSE::Vulkan
{
	struct DescriptorSetLayoutData 
	{
		uint32_t set_number;
		VkDescriptorSetLayoutCreateInfo create_info;
		std::vector<VkDescriptorSetLayoutBinding> bindings;
	};

	class VulkanShaderModule
	{
	private:
		VkShaderModule shaderModule;
		Graphics::ShaderModuleType moduleType;
		SpvReflectShaderModule reflectionModule;

	public:
		bool create(const std::vector<byte>& code, Graphics::ShaderModuleType _moduleType);

		void destroy();

		Graphics::ShaderModuleType getModuleType() const;
		VkShaderModule getModule() const;

		VertexFormat getInputFormat() const;
		std::vector<DescriptorSetLayoutData> getDescriptorSetLayoutData() const;
		const char* getEntryPoint() const;
	};
}

#endif