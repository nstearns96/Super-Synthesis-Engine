#include "Graphics/Shaders/Shader.h"

#include <algorithm>

#include "EngineTypeDefs.h"

#include "Graphics/Shaders/ShaderModuleType.h"

#include "Logging/Logger.h"

#include "Vulkan/Devices/VulkanDeviceManager.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Graphics
	{
		bool Shader::create(const std::vector<std::string>& sourceFiles, const std::vector<ShaderModuleType>& moduleTypes)
		{
			for (st i = 0; i < sourceFiles.size(); ++i)
			{
				SSE::FileHandle sourceFile;
				sourceFile.create("Source\\Shaders\\" + sourceFiles[i], FioMode::FIOM_BINARY | FioMode::FIOM_READ);

				const std::vector<byte> source = sourceFile.readIntoVector();

				Vulkan::VulkanShaderModule shaderModule;
				if (!shaderModule.create(source, moduleTypes[i]))
				{
					destroy();
					GLOG_CRITICAL("Failed to create shader module.");
					return false;
				}

				shaderModules.push_back(shaderModule);
			}

			std::vector<Vulkan::DescriptorSetLayoutData> descriptorSetLayoutData;
			for (const auto& shaderModule : shaderModules)
			{
				auto shaderLayoutData = shaderModule.getDescriptorSetLayoutData();
				descriptorSetLayoutData.insert(descriptorSetLayoutData.end(), shaderLayoutData.begin(), shaderLayoutData.end());
			}

			for (auto& layoutData : descriptorSetLayoutData)
			{
				layoutData.create_info.pBindings = layoutData.bindings.data();
			}

			std::sort
			(
				descriptorSetLayoutData.begin(), descriptorSetLayoutData.end(),
				[](const Vulkan::DescriptorSetLayoutData& a, const Vulkan::DescriptorSetLayoutData& b)
				{
					return a.set_number < b.set_number; 
				}
			);

			descriptorSetLayouts.resize(descriptorSetLayoutData.size());
			for (st d = 0; d < descriptorSetLayoutData.size(); ++d)
			{
				if (vkCreateDescriptorSetLayout(LOGICAL_DEVICE_DEVICE, &descriptorSetLayoutData[d].create_info, nullptr, &descriptorSetLayouts[d]) != VK_SUCCESS)
				{
					GLOG_CRITICAL("Failed to create descriptor layout.");
					return false;
				}
			}

			return true;
		}

		void Shader::destroy()
		{
			for (st i = 0; i < shaderModules.size(); ++i)
			{
				shaderModules[i].destroy();
			}

			for (st d = 0; d < descriptorSetLayouts.size(); ++d)
			{
				vkDestroyDescriptorSetLayout(LOGICAL_DEVICE_DEVICE, descriptorSetLayouts[d], nullptr);
			}
		}

		Vulkan::VulkanShaderModule Shader::getShaderModule(ShaderModuleType moduleType) const
		{
			for (st i = 0; i < shaderModules.size(); ++i)
			{
				if (shaderModules[i].getModuleType() == moduleType)
				{
					return shaderModules[i];
				}
			}

			return shaderModules[0];
		}

		std::vector<VkPipelineShaderStageCreateInfo> Shader::getShaderStages() const
		{
			std::vector<VkPipelineShaderStageCreateInfo> result;
			
			for (auto shaderModule : shaderModules)
			{
				VkShaderStageFlagBits stageFlag;
				switch (shaderModule.getModuleType())
				{
				case ShaderModuleType::SMT_VERTEX:
				{
					stageFlag = VK_SHADER_STAGE_VERTEX_BIT;
					break;
				}
				case ShaderModuleType::SMT_FRAGMENT:
				{
					stageFlag = VK_SHADER_STAGE_FRAGMENT_BIT;
					break;
				}
				default:
				{
					stageFlag = VK_SHADER_STAGE_ALL;
					break;
				}
				}
				VkPipelineShaderStageCreateInfo shaderStageInfo{};
				shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				shaderStageInfo.stage = stageFlag;
				shaderStageInfo.module = shaderModule.getModule();
				shaderStageInfo.pName = shaderModule.getEntryPoint();

				result.push_back(shaderStageInfo);
			}

			return result;
		}

		VertexFormat Shader::getInputFormat() const
		{
			return getShaderModule(SMT_VERTEX).getInputFormat();
		}

		std::vector<VkDescriptorSetLayout> Shader::getDescriptorSetLayouts() const
		{
			return descriptorSetLayouts;
		}
	}
}