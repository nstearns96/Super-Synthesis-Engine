#include "VulkanShaderModule.h"

#include "VulkanDeviceManager.h"
#include "Logger.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Vulkan
	{
		bool VulkanShaderModule::create(const std::vector<char>& code, ShaderModuleType _moduleType)
		{
			VkShaderModuleCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = code.size();
			createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

			if (vkCreateShaderModule(VulkanDeviceManager::gDeviceManager.getActiveLogicalDevice().getDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) 
			{
				gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to create shader module.");
				return false;
			}

			moduleType = _moduleType;

			return true;
		}

		void VulkanShaderModule::destroy()
		{
			vkDestroyShaderModule(VulkanDeviceManager::gDeviceManager.getActiveLogicalDevice().getDevice(), shaderModule, nullptr);
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