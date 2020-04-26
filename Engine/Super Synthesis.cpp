#include "Super Synthesis.h"

#include <SDL/SDL.h>
#include <SDL/SDL_vulkan.h>

#include "Logger.h"
#include "WindowManager.h"
#include "Vulkan.h"
#include "GraphicsPipeline.h"
#include "VulkanDeviceManager.h"
#include "VulkanSurface.h"

SSE::Vulkan::VulkanSurface* gSurface = nullptr;
SSE::Graphics::GraphicsPipeline* gPipeline = nullptr;

namespace SSE
{
	extern Logger gLogger;

	bool initSubSystems()
	{
		gLogger.openLog("log.txt");

		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to intialize SDL!");
			return false;
		}

		//Fuck SDL
		if (SDL_Vulkan_LoadLibrary(nullptr) != NULL)
		{
			gLogger.logError(ErrorLevel::EL_CRITICAL, SDL_GetError());
			return false;
		}

		WindowManager::gWindowManager.createWindow("main", "Super Synthesis Engine", glm::vec2(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED), glm::vec2(1920, 1080), SDL_WINDOW_SHOWN, true);

		gSurface = new Vulkan::VulkanSurface;
		Vulkan::initVulkan(WindowManager::gWindowManager.getActiveWindow(), *gSurface);

		gPipeline = new Graphics::GraphicsPipeline;
		gPipeline->create(*gSurface);

		return true;
	}

	void mainLoop()
	{
		bool isRunning = true;
		while (isRunning)
		{
			SDL_Event ev;
			while (SDL_PollEvent(&ev))
			{
				switch (ev.type)
				{
					case SDL_QUIT:
					{
						isRunning = false;
						break;
					}
				}
			}

			gPipeline->executeRenderPass();
		}

		vkDeviceWaitIdle(Vulkan::VulkanDeviceManager::gDeviceManager.getActiveLogicalDevice().getDevice());
	}

	void cleanup()
	{
		gPipeline->destroy();
		delete gPipeline;

		gSurface->destroy();
		delete gSurface;

		Vulkan::cleanupVulkan();

		WindowManager::gWindowManager.clear();

		SDL_Vulkan_UnloadLibrary();

		SDL_Quit();

		gLogger.closeLog();
	}
}