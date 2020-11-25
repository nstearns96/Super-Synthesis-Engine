#include "Input/InputManager.h"

#include "Logging/Logger.h"

namespace SSE
{
	extern Logger gLogger;

	namespace Input
	{
		KeyboardInput InputManager::keyboard;
		MouseInput InputManager::mouse;

		void InputManager::update()
		{
			if (!keyboard.newKeyStates)
			{
				keyboard.newKeyStates = SDL_GetKeyboardState(NULL);
			}

			memcpy(keyboard.oldKeyStates, keyboard.newKeyStates, sizeof(u8) * SDL_NUM_SCANCODES);
			
			SDL_PumpEvents();

			keyboard.modState = SDL_GetModState();

			mouse.oldButtonStates = mouse.newButtonStates;
			mouse.oldCursorPos = mouse.newCursorPos;
			if (mouse.mode == MIM_GLOBAL)
			{
				mouse.newButtonStates = SDL_GetGlobalMouseState(&mouse.newCursorPos.x, &mouse.newCursorPos.y);
			}
			else if (mouse.mode == MIM_RELATIVE)
			{
				mouse.newButtonStates = SDL_GetMouseState(&mouse.newCursorPos.x, &mouse.newCursorPos.y);
			}
		}

		bool InputManager::checkKeyState(SDL_Scancode keyCode, KeyState keyState)
		{
			bool isDown = keyboard.newKeyStates[keyCode] == SDL_TRUE;
			bool wasDown = keyboard.oldKeyStates[keyCode] == SDL_TRUE;

			switch (keyState)
			{
			default:
			case KS_IS_DOWN:
				return isDown;
			case KS_WAS_DOWN:
				return wasDown;
			case KS_UNTOUCHED:
				return !isDown && !wasDown;
			case KS_PRESSED:
				return isDown && !wasDown;
			case KS_RELEASED:
				return !isDown && wasDown;
			case KS_HELD:
				return isDown && wasDown;
			}
		}

		bool InputManager::checkMouseKeyState(MouseButton button, KeyState keyState)
		{
			bool isDown = (mouse.newButtonStates & (u32)SDL_BUTTON(button)) == (u32)SDL_BUTTON(button);
			bool wasDown = (mouse.oldButtonStates & (u32)SDL_BUTTON(button)) == (u32)SDL_BUTTON(button);

			switch (keyState)
			{
			default:
			case KS_IS_DOWN:
				return isDown;
			case KS_WAS_DOWN:
				return wasDown;
			case KS_UNTOUCHED:
				return !isDown && !wasDown;
			case KS_PRESSED:
				return isDown && !wasDown;
			case KS_RELEASED:
				return !isDown && wasDown;
			case KS_HELD:
				return isDown && wasDown;
			}
		}

		glm::ivec2 InputManager::getMousePosition()
		{
			return mouse.newCursorPos;
		}

		glm::ivec2 InputManager::getPreviousMousePosition()
		{
			return mouse.oldCursorPos;
		}

		glm::ivec2 InputManager::getMouseDelta()
		{
			return mouse.newCursorPos - mouse.oldCursorPos;
		}
	}
}