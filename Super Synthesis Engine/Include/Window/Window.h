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

#include "EngineTypeDefs.h"

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

		glm::uvec2 dimensions;
		glm::uvec2 position;

		u32 id = 0;
		bitfield status = 0x0;

	public:
		Window(const char* title, const glm::uvec2& _position = glm::uvec2(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED),
			const glm::uvec2& _dimensions = glm::uvec2(1920,1080), bitfield flags = WINDOW_SHOWN);
		Window() {};
		~Window();

		SDL_Window* getWindow();
		bitfield getStatus();
		void setStatus(bitfield flags, bool setFlags);
		void setStatus(bitfield status);
		u32 getID();

		u32 getWidth();
		u32 getHeight();

		WindowEventResult handleEvent(const SDL_Event& e);

	};
}

#endif