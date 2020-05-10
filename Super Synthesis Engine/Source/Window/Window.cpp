#include "Window/Window.h"
#include "Error/Error.h"
#include "Logging/Logger.h"

namespace SSE
{
	extern Logger gLogger;

	Window::Window(const char* title, glm::vec2 _position, glm::vec2 _dimensions, unsigned int flags)
	{
		window = SDL_CreateWindow(title, _position.x, _position.y, _dimensions.x, _dimensions.y, SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
		if (window == NULL) {
			gLogger.logError(ErrorLevel::EL_CRITICAL, "Failed to open window. " + std::string(SDL_GetError()));
		}
		else
		{
			id = SDL_GetWindowID(window);
			int x, y;
			SDL_GetWindowPosition(window, &x, &y);
			position = glm::vec2{x,y};
			dimensions = _dimensions;
			status = flags;
		}
	}

	Window::~Window()
	{}

	SDL_Window* Window::getWindow()
	{
		return window;
	}

	unsigned int Window::getID()
	{
		return id;
	}

	unsigned int Window::getStatus()
	{
		return status;
	}

	void Window::setStatus(unsigned int flags, bool setFlags)
	{
		status = setFlags ? (status | flags) : (status & (~flags));
	}

	void Window::setStatus(unsigned int flags)
	{
		status = flags;
	}

	int Window::getWidth()
	{
		return dimensions.x;
	}

	int Window::getHeight()
	{
		return dimensions.y;
	}

	WindowEventResult Window::handleEvent(SDL_Event &e)
	{
		switch (e.window.event)
		{
		case SDL_WINDOWEVENT_SHOWN:
		{
			setStatus(WINDOW_SHOWN, true);
			return RESULT_NONE;
			break;
		}
		case SDL_WINDOWEVENT_HIDDEN:
		{
			setStatus(WINDOW_SHOWN, false);
			return RESULT_NONE;
			break;
		}
		case SDL_WINDOWEVENT_SIZE_CHANGED:
		case SDL_WINDOWEVENT_RESIZED:
		{
			int w;
			int h;
			SDL_GetWindowSize(window, &w, &h);

			if (w != dimensions.x || h != dimensions.y)
			{
				dimensions = { w,h };
				return RESULT_SIZE_CHANGE;
			}

			return RESULT_NONE;
			break;
		}
		case SDL_WINDOWEVENT_EXPOSED:
		{
			return RESULT_NONE;
			break;
		}
		case SDL_WINDOWEVENT_ENTER:
		{
			setStatus(WINDOW_MOUSE_FOCUS, true);
			return RESULT_NONE;
			break;
		}
		case SDL_WINDOWEVENT_LEAVE:
		{
			setStatus(WINDOW_MOUSE_FOCUS, false);
			return RESULT_NONE;
			break;
		}
		case SDL_WINDOWEVENT_FOCUS_GAINED:
		{
			setStatus(WINDOW_KEYBOARD_FOCUS, true);
			return RESULT_NONE;
			break;
		}
		case SDL_WINDOWEVENT_FOCUS_LOST:
		{
			setStatus(WINDOW_KEYBOARD_FOCUS, false);
			return RESULT_NONE;
			break;
		}
		case SDL_WINDOWEVENT_MINIMIZED:
		{
			setStatus(WINDOW_MINIMIZED, true);
			return RESULT_NONE;
			break;
		}
		case SDL_WINDOWEVENT_MAXIMIZED:
		{
			setStatus(WINDOW_MINIMIZED, false);
			return RESULT_NONE;
			break;
		}
		case SDL_WINDOWEVENT_RESTORED:
		{
			setStatus(WINDOW_MINIMIZED, false);
			return RESULT_NONE;
			break;
		}
		case SDL_WINDOWEVENT_CLOSE:
		{
			SDL_HideWindow(window);
			return RESULT_WINDOW_CLOSE;
			break;
		}
		}
	}
}