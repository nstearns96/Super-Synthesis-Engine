#ifndef _SSE_INPUT_MANAGER_H
#define _SSE_INPUT_MANAGER_H

#include <glm/glm.hpp>
#include <SDL/SDL.h>

#include "EngineTypeDefs.h"

namespace SSE::Input
{
	enum MouseInputMode
	{
		MIM_GLOBAL,
		MIM_RELATIVE
	};

	enum KeyState
	{
		KS_IS_DOWN,
		KS_WAS_DOWN,
		KS_UNTOUCHED,
		KS_PRESSED,
		KS_RELEASED,
		KS_HELD
	};

	enum MouseButton
	{
		MOUSE_LEFT,
		MOUSE_MIDDLE,
		MOUSE_RIGHT
	};

	struct KeyboardInput
	{
		const u8* newKeyStates;
		u8 oldKeyStates[SDL_NUM_SCANCODES];
		SDL_Keymod modState;
	};

	struct MouseInput
	{
		glm::ivec2 newCursorPos;
		u32 newButtonStates;
		glm::ivec2 oldCursorPos;
		u32 oldButtonStates;
		MouseInputMode mode;
	};

	class InputManager
	{
	private:
		InputManager() {};

		static KeyboardInput keyboard;
		static MouseInput mouse;

	public:
		static void update();

		static bool checkKeyState(SDL_Scancode keyCode, KeyState keyState);
		static bool checkMouseKeyState(MouseButton button, KeyState keyState);

		static glm::ivec2 getMousePosition();
		static glm::ivec2 getPreviousMousePosition();
		static glm::ivec2 getMouseDelta();
	};
}

#endif