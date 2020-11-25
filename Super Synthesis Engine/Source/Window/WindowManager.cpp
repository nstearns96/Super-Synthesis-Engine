#include "Window/WindowManager.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Error/Error.h"

#include "Logging/Logger.h"

namespace SSE
{
	extern Logger gLogger;

	WindowManager WindowManager::gWindowManager;

	WindowManager::WindowManager() {};

	bool WindowManager::createWindow(const std::string& windowName, const char* title, const glm::uvec2& position, const glm::uvec2& dimensions, bitfield flags, bool setActive)
	{
		if ((windows.find(windowName) != windows.end()))
		{
			GLOG_WARNING("Window with the name, \"" + windowName + "\" already exists! Window not created.");
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

		GLOG_WARNING("Cannot find window, \"" + windowName + "\".");
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
			return true;
		}
		else
		{
			GLOG_WARNING("Cannot find window, \"" + windowName + "\".");
			return false;
		}
	}

	void WindowManager::clear()
	{
		for (auto& window : windows)
		{
			window.second.destroy();
		}
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
}