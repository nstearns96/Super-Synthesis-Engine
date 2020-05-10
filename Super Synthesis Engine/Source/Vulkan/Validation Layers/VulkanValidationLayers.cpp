#include "Vulkan/Validation Layers/VulkanValidationLayers.h"

#include "Vulkan/VulkanInstance.h"
#include "Logging/Logger.h"

namespace SSE
{
	extern Logger gLogger;
}

namespace SSE::Vulkan::ValidationLayers
{
	VkDebugUtilsMessengerEXT debugMessenger;

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback
	(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	)
	{
#pragma message("TODO: Translate Vulkan severity to SSE severity")
		gLogger.logError(ErrorLevel::EL_CRITICAL, pCallbackData->pMessage);

		return VK_FALSE;
	}

	bool checkValidationLayerSupport()
	{
		unsigned int layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> layers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

		bool layersFound = true;

		for (const char* layerName : getRequiredLayers())
		{
			bool layerFound = false;
			for (const auto& layer : layers)
			{
				if (strcmp(layer.layerName, layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
			{
				layersFound = false;
				break;
			}
		}

		return layersFound;
	}

	void initDebugExtensions()
	{
		if (!enableValidationLayers) return;

		VkInstance instance = VulkanInstance::gInstance.getInstance();

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		getDebugMessengerCreateInfo(createInfo);

		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func == nullptr || (func(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS))
		{
			gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to create VkDebugUtilsMessengerEXT");
		}
	}

	void cleanupDebugExtensions()
	{
		if (!enableValidationLayers) return;

		VkInstance instance = VulkanInstance::gInstance.getInstance();

		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			func(instance, debugMessenger, nullptr);
		}
		else
		{
			gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to destroy VkDebugUtilsMessengerEXT");
		}
	}

	std::vector<const char*> getRequiredExtensions()
	{
		unsigned int count;
		std::vector<const char*> extensions;
		if (SDL_Vulkan_GetInstanceExtensions(nullptr, &count, nullptr) != SDL_TRUE)
		{
			gLogger.logError(ErrorLevel::EL_CRITICAL, SDL_GetError());
		}
		
		extensions.resize(count);

		SDL_Vulkan_GetInstanceExtensions(nullptr, &count, extensions.data());
		std::vector<const char*> requiredExtensions(extensions.begin(), extensions.begin() + count);

		if (enableValidationLayers) 
		{
			requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return requiredExtensions;
	}

	std::vector<const char*> getRequiredLayers()
	{
		std::vector<const char*> layers;

		if (enableValidationLayers)
		{
			layers.push_back("VK_LAYER_KHRONOS_validation");
		}

		return layers;
	}

	void getDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
	{
		if (!enableValidationLayers) return;

		createInfo = {};

		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
		createInfo.pUserData = nullptr;
	}
}