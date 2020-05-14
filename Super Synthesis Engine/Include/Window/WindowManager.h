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

		bool createWindow(const std::string& windowName, const char* title, const glm::vec2& position, const glm::vec2& dimensions, unsigned int flags, bool setActive);
		bool setActiveWindow(const std::string& windowName);
		const Window* getWindow(const std::string& windowName) const;
		Window& getActiveWindow() const;
		bool destroyWindow(const std::string& windowName);

		glm::vec2 getWindowFrameBufferDimensions() const;

		unsigned int getWindowCount() const;
		void clear();
		unsigned int handleEvents(const SDL_Event& e);;
	};

}

#endif