#include "Super Synthesis.h"

#include <chrono>

#include <SDL/SDL.h>
#include <SDL/SDL_vulkan.h>

#include "Audio/AudioManager.h"

#include "Graphics/GraphicsPipeline.h"
#include "Graphics/Camera.h"
#include "Graphics/Renderer.h"

#include "Graphics/Shaders/ShaderModuleType.h"

#include "Input/InputManager.h"

#include "Logging/Logger.h"

#include "Object.h"

#include "Resources/ResourceManager.h"

#include "Time/Timer.h"

#include "Vulkan/VulkanMain.h"
#include "Vulkan/VulkanInstance.h"
#include "Vulkan/Devices/VulkanDeviceManager.h"

#include "Window/WindowManager.h"

constexpr r32 CAMERA_ROTATE_SPEED = 5.0f;
constexpr r32 CAMERA_MOVE_SPEED = 5.0f;

SSE::Timer gTimer;
std::vector<SSE::Object> gObjects;

namespace SSE
{
	extern Logger gLogger;

	bool initSubSystems()
	{
		Timer::initTime();
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

		Vulkan::VulkanInstance::gInstance.create();

		WindowManager::gWindowManager.createWindow("main", "Super Synthesis Engine", glm::vec2(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED), glm::vec2(1280, 720), SDL_WINDOW_SHOWN, true);

		Vulkan::initVulkan();
		Vulkan::gDescriptorPool.create();

		ResourceManager::loadTexture("viking_room.bmp");

		ResourceHandle shader = ResourceManager::loadShader({ "frag.spv","vert.spv" }, { Graphics::ShaderModuleType::SMT_FRAGMENT, Graphics::ShaderModuleType::SMT_VERTEX });

		ResourceManager::loadAudio("sine.wav");

		ResourceManager::loadModel("viking_room.obj");

		AudioManager::findDevice("mainAudio");
		//AudioManager::play(ResourceManager::getAudio(0));

		gCamera.create(glm::vec3(-3.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, -0.3333f));
		Graphics::Renderer::gRenderer.create();
		Graphics::Renderer::gRenderer.createPipeline(shader);
		Graphics::Renderer::gRenderer.setPipeline(0);
		Graphics::Renderer::gRenderer.setCamera(&gCamera);

		return true;
	}

	void mainLoop()
	{
		bool isRunning = true;
		while (isRunning)
		{
			Input::InputManager::update();

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
							Graphics::Renderer::gRenderer.isFrameBufferDirty = true;
						}
						}
					}
				}
			}
			gTimer.endTime();
			r32 deltaTime = (r32)gTimer.getTime();
			gTimer.startTime();
						
			glm::ivec2 mouseDelta = Input::InputManager::getMouseDelta();
			gCamera.rotateByAxes({ mouseDelta.x * CAMERA_ROTATE_SPEED * deltaTime, mouseDelta.y * CAMERA_ROTATE_SPEED * deltaTime });

			if (Input::InputManager::checkKeyState(SDL_SCANCODE_W, Input::KS_IS_DOWN))
			{
				gCamera.translate(gCamera.getForwardVector() * CAMERA_MOVE_SPEED * deltaTime);
			}
			if (Input::InputManager::checkKeyState(SDL_SCANCODE_S, Input::KS_IS_DOWN))
			{
				gCamera.translate(-gCamera.getForwardVector() * CAMERA_MOVE_SPEED * deltaTime);
			}
			if (Input::InputManager::checkKeyState(SDL_SCANCODE_A, Input::KS_IS_DOWN))
			{
				gCamera.translate(-gCamera.getRightVector() * CAMERA_MOVE_SPEED * deltaTime);
			}
			if (Input::InputManager::checkKeyState(SDL_SCANCODE_D, Input::KS_IS_DOWN))
			{
 				gCamera.translate(gCamera.getRightVector() * CAMERA_MOVE_SPEED * deltaTime);
			}
   			if (Input::InputManager::checkKeyState(SDL_SCANCODE_SPACE, Input::KS_PRESSED))
			{
				Object newObject;
				newObject.create(0, 0);
				newObject.transform(glm::vec3((float)gObjects.size(), 0.0f, 0.0f), glm::vec3(1.0f), glm::mat4(1.0f));
				gObjects.push_back(newObject);
			}

			for (const auto& object : gObjects)
			{
				Graphics::Renderer::gRenderer.renderObject(object);
			}

			gCamera.update();

			AudioManager::update();
			Graphics::Renderer::gRenderer.executeRenderPass();
		}
	}

	void cleanup()
	{
		AudioManager::clear();
		
		Graphics::Renderer::gRenderer.destroy();
		ResourceManager::clear();
		gCamera.destroy();
		
		for (auto& object : gObjects)
		{
			object.destroy();
		}
		gObjects.clear();

		Vulkan::gDescriptorPool.destroy();
		WindowManager::gWindowManager.clear();

		Vulkan::cleanupVulkan();

		SDL_Vulkan_UnloadLibrary();

		SDL_Quit();

		gLogger.closeLog();
	}
}