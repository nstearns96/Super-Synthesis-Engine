#include "Window/WindowManager.h"

#include <SDL/SDL.h>
#include <SDL/SDL_vulkan.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Logging/Logger.h"
#include "Error/Error.h"

namespace SSE
{
	extern Logger gLogger;

	WindowManager WindowManager::gWindowManager;

	WindowManager::WindowManager() {};

	bool WindowManager::createWindow(const std::string& windowName, const char* title, const glm::uvec2& position, const glm::uvec2& dimensions, bitfield flags, bool setActive)
	{
		if ((windows.find(windowName) != windows.end()))
		{
			gLogger.logError(ErrorLevel::EL_WARNING, "Window with the name, \"" + windowName + "\" already exists! Window not created.");
			return false;
		}

		Window newWindow = Window(title, position, dimensions, flags);
		if (newWindow.getID() == 0)
		{
			return false;
		}

		windows[windowName] = newWindow;
		
		if (setActive)
		{
			setActiveWindow(windowName);
		}

		return true;
	}

	bool WindowManager::setActiveWindow(const std::string& windowName)
	{
		auto windowIter = windows.find(windowName);
		if (windowIter != windows.end())
		{
			activeWindow = windowIter;
			Window active = windows[windowName];

			return true;
		}

		gLogger.logError(ErrorLevel::EL_WARNING, "Cannot find window, \"" + windowName + "\".");
		return false;
	}

	const Window* WindowManager::getWindow(const std::string& windowName) const
	{
		auto windowIter = windows.find(windowName);
		if (windowIter == windows.end())
		{
			return nullptr;
		}

		return &windowIter->second;
	}

	bool WindowManager::destroyWindow(const std::string& windowName)
	{
		auto windowIter = windows.find(windowName);
		if (windowIter != windows.end())
		{
			if (windowIter == activeWindow)
			{
				activeWindow = windows.begin();
			}
			windows.erase(windowName);
			return true;
		}
		else
		{
			gLogger.logError(ErrorLevel::EL_WARNING, "Cannot find window, \"" + windowName + "\".");
			return false;
		}
	}

	void WindowManager::clear()
	{
		windows.clear();
	}

	WindowEventResult WindowManager::handleEvents(const SDL_Event& e)
	{
		WindowEventResult result = WindowEventResult::RESULT_NONE;

		for (auto& iter : windows)
		{
			if (e.window.windowID == iter.second.getID())
			{
				result = iter.second.handleEvent(e);
				switch (result)
				{
				case WindowEventResult::RESULT_WINDOW_CLOSE:
				{
					destroyWindow(iter.first);
					break;
				}
				}
				break;
			}
		}

		return result;
	}

	st WindowManager::getWindowCount() const
	{
		return windows.size();
	}

	Window& WindowManager::getActiveWindow() const
	{
		return (activeWindow->second);
	}

	glm::uvec2 WindowManager::getWindowFrameBufferDimensions() const
	{
		glm::uvec2 result;
		Window window = getActiveWindow();
		
		SDL_Vulkan_GetDrawableSize(WindowManager::gWindowManager.getActiveWindow().getWindow(), (i32 *)&result.x, (i32 *)&result.y);

		return result;
	}
}