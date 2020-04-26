#include "Shader.h"

#include "Logger.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Graphics
	{
		bool Shader::create(const std::vector<std::string>& sourceFiles, const std::vector<Vulkan::ShaderModuleType>& moduleTypes)
		{
			for (int i = 0; i < sourceFiles.size(); ++i)
			{
				SSE::FileHandle sourceFile;
				sourceFile.open("Shaders\\" + sourceFiles[i], FioMode::FIOM_BINARY | FioMode::FIOM_READ);

				const std::vector<char> source = sourceFile.readIntoVector();

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
			for (int i = 0; i < shaderModules.size(); ++i)
			{
				shaderModules[i].destroy();
			}
		}

		Vulkan::VulkanShaderModule Shader::getShaderModule(Vulkan::ShaderModuleType moduleType)
		{
			for (int i = 0; i < shaderModules.size(); ++i)
			{
				if (shaderModules[i].getModuleType() == moduleType)
				{
					return shaderModules[i];
				}
			}

			return shaderModules[0];
		}
	}
}