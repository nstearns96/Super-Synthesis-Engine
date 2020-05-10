#ifndef _SSE_WINDOW_MANAGER_H
#define _SSE_WINDOW_MANAGER_H

#include <map>
#include <glm/glm.hpp>

#include "Window.h"

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

		bool createWindow(std::string windowName, const char* title, glm::vec2 position, glm::vec2 dimensions, unsigned int flags, bool setActive);
		bool setActiveWindow(std::string windowName);
		Window* getWindow(std::string windowName);
		Window& getActiveWindow();
		bool destroyWindow(std::string windowName);


		unsigned int getWindowCount();
		void clear();
		void clearWindows();
		unsigned int handleEvents(SDL_Event &e);;
	};

}

#endif