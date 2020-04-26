#include "VulkanInstance.h"

#include <SDL/SDL_vulkan.h>

#include "Logger.h"
#include "VulkanValidationLayers.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Vulkan
	{
		VulkanInstance VulkanInstance::gInstance;

		VulkanInstance::VulkanInstance() {};

		bool VulkanInstance::create(Window& window)
		{
			VkApplicationInfo appInfo = {};
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appInfo.pApplicationName = "";
			appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.pEngineName = "Super Synthesis Engine";
			appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.apiVersion = VK_API_VERSION_1_0;

			VkInstanceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.pApplicationInfo = &appInfo;

			bool validationLayers = Vulkan::ValidationLayers::enableValidationLayers;
			if (validationLayers && !(Vulkan::ValidationLayers::checkValidationLayerSupport()))
			{
				gLogger.logError(ErrorLevel::EL_CRITICAL, "Validation layers requested were not available.");
				return false;
			}

			std::vector<const char*> layers = Vulkan::ValidationLayers::getRequiredLayers();

			createInfo.enabledLayerCount = layers.size();
			createInfo.ppEnabledLayerNames = layers.data();

			auto extensions = Vulkan::ValidationLayers::getRequiredExtensions();
			createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
			createInfo.ppEnabledExtensionNames = extensions.data();

			VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
			Vulkan::ValidationLayers::getDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = &debugCreateInfo;

			VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
			if (result != VK_SUCCESS)
			{
				gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to create Vulkan instance.");
				return false;
			}

			return true;
		}

		void VulkanInstance::destroy()
		{
			Vulkan::ValidationLayers::cleanupDebugExtensions();

			vkDestroyInstance(instance, nullptr);
		}

		VkInstance VulkanInstance::getInstance()
		{
			return instance;
		}
	}
}