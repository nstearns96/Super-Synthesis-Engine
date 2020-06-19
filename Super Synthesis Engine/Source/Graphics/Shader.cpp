#include "Graphics/Shader.h"

#include "EngineTypeDefs.h"
#include "Logging/Logger.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Graphics
	{
		bool Shader::create(const std::vector<std::string>& sourceFiles, const std::vector<Vulkan::ShaderModuleType>& moduleTypes)
		{
			for (st i = 0; i < sourceFiles.size(); ++i)
			{
				SSE::FileHandle sourceFile;
				sourceFile.open("Source\\Shaders\\" + sourceFiles[i], FioMode::FIOM_BINARY | FioMode::FIOM_READ);

				const std::vector<byte> source = sourceFile.readIntoVector();

				Vulkan::VulkanShaderModule shaderModule;
				if (!shaderModule.create(source, moduleTypes[i]))
				{
					gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to create shader module.");
					return false;
				}

				shaderModules.push_back(shaderModule);
			}

			return true;
		}

		void Shader::destroy()
		{
			for (st i = 0; i < shaderModules.size(); ++i)
			{
				shaderModules[i].destroy();
			}
		}

		Vulkan::VulkanShaderModule Shader::getShaderModule(Vulkan::ShaderModuleType moduleType)
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

		std::vector<VkPipelineShaderStageCreateInfo> Shader::getShaderStages()
		{
			std::vector<VkPipelineShaderStageCreateInfo> result;
			
			for (auto shaderModule : shaderModules)
			{
				VkShaderStageFlagBits stageFlag;
				switch (shaderModule.getModuleType())
				{
				case Vulkan::ShaderModuleType::SMT_VERTEX:
				{
					stageFlag = VK_SHADER_STAGE_VERTEX_BIT;
					break;
				}
				case Vulkan::ShaderModuleType::SMT_FRAGMENT:
				{
					stageFlag = VK_SHADER_STAGE_FRAGMENT_BIT;
					break;
				}
				default:
				{
					stageFlag = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
					break;
				}
				}
				VkPipelineShaderStageCreateInfo shaderStageInfo{};
				shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				shaderStageInfo.stage = stageFlag;
				shaderStageInfo.module = shaderModule.getModule();
				shaderStageInfo.pName = "main";

				result.push_back(shaderStageInfo);
			}

			return result;
		}
	}
}