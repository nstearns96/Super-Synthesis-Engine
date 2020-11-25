#ifndef _SSE_WINDOW_MANAGER_H
#define _SSE_WINDOW_MANAGER_H

#include <map>

#include <glm/glm.hpp>

#include "Window/Window.h"

namespace SSE
{
	class WindowManager
	{
	private:
		std::map<std::string, Window> windows;
		std::map<std::string, Window>::iterator activeWindow;

		WindowManager();

	public:

		static WindowManager gWindowManager;

		bool createWindow(const std::string& windowName, const char* title, const glm::uvec2& position, const glm::uvec2& dimensions, u32 flags, bool setActive);
		bool setActiveWindow(const std::string& windowName);
		const Window* getWindow(const std::string& windowName) const;
		Window& getActiveWindow() const;
		bool destroyWindow(const std::string& windowName);

		st getWindowCount() const;
		void clear();
		WindowEventResult handleEvents(const SDL_Event& e);;
	};

}

#endif