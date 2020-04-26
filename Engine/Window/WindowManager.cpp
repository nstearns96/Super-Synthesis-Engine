#include "WindowManager.h"

#include <SDL/SDL.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Logger.h"
#include "Error.h"

namespace SSE
{
	extern Logger gLogger;

	WindowManager WindowManager::gWindowManager;

	WindowManager::WindowManager() {};

	bool WindowManager::createWindow(std::string windowName, const char* title, glm::vec2 position, glm::vec2 dimensions, unsigned int flags, bool setActive)
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
	}

	bool WindowManager::setActiveWindow(std::string windowName)
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

	bool WindowManager::destroyWindow(std::string windowName)
	{
		if ((windows.find(windowName) != windows.end()))
		{
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

	void WindowManager::clearWindows()
	{

	}

	void WindowManager::handleEvents(SDL_Event &e)
	{
		for (auto iter : windows)
		{
			if (e.window.windowID == iter.second.getID())
			{
				if (iter.second.handleEvent(e) == WindowEventResult::RESULT_WINDOW_CLOSE)
				{
					destroyWindow(iter.first);
				}
				break;
			}
		}
	}

	Window& WindowManager::getActiveWindow()
	{
		return (activeWindow->second);
	}
}