#include "Super Synthesis.h"

#include <SDL/SDL.h>
#include <SDL/SDL_vulkan.h>

#include "Logging/Logger.h"
#include "Window/WindowManager.h"
#include "Vulkan/VulkanMain.h"
#include "Graphics/GraphicsPipeline.h"
#include "Vulkan/Devices/VulkanDeviceManager.h"
#include "Vulkan/Graphics/VulkanSurface.h"
#include "Resources/ResourceManager.h"

SSE::Graphics::GraphicsPipeline* gPipeline = nullptr;

namespace SSE
{
	extern Logger gLogger;

	bool initSubSystems()
	{
		gLogger.openLog("log.txt");

		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			GLOG_CRITICAL("Failed to initialize SDL!");
			return false;
		}

		if (SDL_Vulkan_LoadLibrary(nullptr) != NULL)
		{
			GLOG_CRITICAL(SDL_GetError());
			return false;
		}

		WindowManager::gWindowManager.createWindow("main", "Super Synthesis Engine", glm::vec2(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED), glm::vec2(1280, 720), SDL_WINDOW_SHOWN, true);

		gPipeline = new Graphics::GraphicsPipeline;
		Vulkan::initVulkan(WindowManager::gWindowManager.getActiveWindow(), gPipeline->surface);

		ResourceManager::loadTexture("texture", "test.bmp");
		ResourceManager::loadShader("main", { "frag.spv","vert.spv" }, { Vulkan::ShaderModuleType::SMT_FRAGMENT, Vulkan::ShaderModuleType::SMT_VERTEX });

		gPipeline->create();

		return true;
	}

	void mainLoop()
	{
		bool isRunning = true;
		while (isRunning)
		{
#pragma message("TODO: Create proper event handle/dispatcher")
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
					case SDL_WINDOWEVENT:
					{
						switch (WindowManager::gWindowManager.handleEvents(ev))
						{
						case WindowEventResult::RESULT_SIZE_CHANGE:
						{
							gPipeline->isFrameBufferDirty = true;
						}
						}
					}
				}
			}

			gPipeline->executeRenderPass();
		}

	}

	void cleanup()
	{
		gPipeline->destroy();
		delete gPipeline;

		ResourceManager::clear();

		Vulkan::cleanupVulkan();

		WindowManager::gWindowManager.clear();

		SDL_Vulkan_UnloadLibrary();

		SDL_Quit();

		gLogger.closeLog();
	}
}