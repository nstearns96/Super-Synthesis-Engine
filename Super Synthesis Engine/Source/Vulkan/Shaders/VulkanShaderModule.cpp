#include "Vulkan/Shaders/VulkanShaderModule.h"

#include "Vulkan/Devices/VulkanDeviceManager.h"
#include "Logging/Logger.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Vulkan
	{
		bool VulkanShaderModule::create(const std::vector<byte>& code, ShaderModuleType _moduleType)
		{
			VkShaderModuleCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = code.size();
			createInfo.pCode = (u32*) code.data();

			if (vkCreateShaderModule(LOGICAL_DEVICE_DEVICE, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) 
			{
				gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to create shader module.");
				return false;
			}

			moduleType = _moduleType;

			return true;
		}

		void VulkanShaderModule::destroy()
		{
			vkDestroyShaderModule(LOGICAL_DEVICE_DEVICE, shaderModule, nullptr);
			moduleType = ShaderModuleType::SMT_NONE;
		}

		ShaderModuleType VulkanShaderModule::getModuleType()
		{
			return moduleType;
		}

		VkShaderModule VulkanShaderModule::getModule()
		{
			return shaderModule;
		}
	}
}