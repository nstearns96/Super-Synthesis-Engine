//====================================================================================
// File: Window.h
// Description: Header file for window backend wrapping and engine-level window functions
//====================================================================================
#ifndef _SSE_WINDOW_H
#define _SSE_WINDOW_H

#include <glm/glm.hpp>
#include <SDL/SDL_video.h>
#include <SDL/SDL_events.h>
#include <string>

namespace SSE
{
	enum WindowEventResult
	{
		RESULT_NONE = 0x0,
		RESULT_WINDOW_CLOSE,
		RESULT_SIZE_CHANGE
	};

	enum WindowStatusFlags
	{
		WINDOW_SHOWN = 0x1,
		WINDOW_MOUSE_FOCUS = 0x2,
		WINDOW_KEYBOARD_FOCUS = 0x4,
		WINDOW_FULLSCREEN = 0x8,
		WINDOW_MINIMIZED = 0x10
	};

	class Window
	{
	private:
		SDL_Window* window = nullptr;

		glm::vec2 dimensions;
		glm::vec2 position;

		unsigned int id = 0;
		unsigned int status = 0x0;

	public:
		Window(const char* title, glm::vec2 position = glm::vec2(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED),
			glm::vec2 dimensions = glm::vec2(1920,1080), unsigned int flags = WINDOW_SHOWN);
		Window() {};
		~Window();

		SDL_Window* getWindow();
		unsigned int getStatus();
		void setStatus(unsigned int flags, bool setFlags);
		void setStatus(unsigned int status);
		unsigned int getID();

		int getWidth();
		int getHeight();

		WindowEventResult handleEvent(const SDL_Event& e);

	};
}

#endif