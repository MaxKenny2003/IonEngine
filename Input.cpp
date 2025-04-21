// gameEngine.cpp
#define GLM_ENABLE_EXPERIMENTAL
#include <string>
#include <iostream>
#include "Input.h"
#include "Renderer.h"
#include "gameEngine.h"

using namespace std;

void Input::Init()
{
	for (int code = SDL_SCANCODE_UNKNOWN; code < SDL_NUM_SCANCODES; code++)
	{
		keyboard_states[static_cast<SDL_Scancode>(code)] = INPUT_STATE_UP;
	}
}

void Input::ProcessEvent(const SDL_Event& e)
{
	if (e.type == SDL_KEYDOWN)
	{
		keyboard_states[e.key.keysym.scancode] = INPUT_STATE_JUST_BECAME_DOWN;
		just_became_down_scancodes.push_back(e.key.keysym.scancode);
	}
	else if (e.type == SDL_KEYUP)
	{
		keyboard_states[e.key.keysym.scancode] = INPUT_STATE_JUST_BECAME_UP;
		just_became_up_scancodes.push_back(e.key.keysym.scancode);
	}
	else if (e.type == SDL_MOUSEMOTION)
	{
		mousePosition.x = static_cast<float>(e.motion.x);
		mousePosition.y = static_cast<float>(e.motion.y);
	}
	else if (e.type == SDL_MOUSEWHEEL)
	{
		mouseWheelDelta = e.wheel.preciseY;
	}
	if (e.type == SDL_MOUSEBUTTONDOWN) {
		mouseState[e.button.button] = INPUT_STATE_JUST_BECAME_DOWN;
		just_became_down_mouseButtons.push_back(e.button.button);
	}

	else if (e.type == SDL_MOUSEBUTTONUP) {
		mouseState[e.button.button] = INPUT_STATE_JUST_BECAME_UP;
		just_became_up_mouseButtons.push_back(e.button.button);
	}
	
}

void Input::LateUpdate()
{
	for (const SDL_Scancode& code : just_became_down_scancodes)
	{
		keyboard_states[code] = INPUT_STATE_DOWN;
	}
	just_became_down_scancodes.clear();

	for (const SDL_Scancode& code : just_became_up_scancodes)
	{
		keyboard_states[code] = INPUT_STATE_UP;
	}
	just_became_up_scancodes.clear();

	for (const uint8_t mouseButton : just_became_down_mouseButtons)
	{
		mouseState[mouseButton] = INPUT_STATE_DOWN;
	}
	just_became_down_mouseButtons.clear();
	for (const uint8_t mouseButton : just_became_up_mouseButtons)
	{
		mouseState[mouseButton] = INPUT_STATE_UP;
	}
	just_became_up_mouseButtons.clear();

	mouseWheelDelta = 0.0f;

}

bool Input::GetKey(SDL_Scancode keycode)
{
	if (keyboard_states[keycode] == INPUT_STATE_DOWN)
	{
		return true;
	}
	if (keyboard_states[keycode] == INPUT_STATE_JUST_BECAME_DOWN)
	{
		return true;
	}
	return false;
}

bool Input::GetKeyDown(SDL_Scancode keycode)
{
	return keyboard_states[keycode] == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetKeyUp(SDL_Scancode keycode)
{
	return keyboard_states[keycode] == INPUT_STATE_JUST_BECAME_UP;
}

bool Input::LuaGetKey(std::string keycode)
{
	if (pointerToSelf->__keycode_to_scancode.find(keycode) != pointerToSelf->__keycode_to_scancode.end())
	{
		// Keycode does exist
		//True if the keycode is down on this frame.
		return GetKey(pointerToSelf->__keycode_to_scancode.at(keycode));
	}
	return false;
}
bool Input::LuaGetKeyDown(std::string keycode)
{
	if (pointerToSelf->__keycode_to_scancode.find(keycode) != pointerToSelf->__keycode_to_scancode.end())
	{
		// Keycode does exist
		return GetKeyDown(pointerToSelf->__keycode_to_scancode.at(keycode));
	}
	return false;
}
bool Input::LuaGetKeyUp(std::string keycode)
{
	if (pointerToSelf->__keycode_to_scancode.find(keycode) != pointerToSelf->__keycode_to_scancode.end())
	{
		// Keycode does exist
		return GetKeyUp(pointerToSelf->__keycode_to_scancode.at(keycode));
	}
	return false;
}

glm::vec2 Input::GetMousePosition()
{
	return mousePosition;
}

bool Input::GetMouseButton(uint8_t buttonNum)
{
	if (pointerToSelf->mouseState[buttonNum] == INPUT_STATE_DOWN)
	{
		return true;
	}
	if (pointerToSelf->mouseState[buttonNum] == INPUT_STATE_JUST_BECAME_DOWN)
	{
		return true;
	}
	return false;
}
bool Input::GetMouseButtonDown(uint8_t buttonNum)
{
	if (pointerToSelf->mouseState[buttonNum] == INPUT_STATE_JUST_BECAME_DOWN)
	{
		return true;
	}
	return false;
}
bool Input::GetMouseButtonUp(uint8_t buttonNum)
{
	if (pointerToSelf->mouseState[buttonNum] == INPUT_STATE_JUST_BECAME_UP)
	{
		return true;
	}
	return false;
}

float Input::GetMouseScrollDelta()
{
	return mouseWheelDelta;
}

void Input::HideCursor()
{
	SDL_ShowCursor(SDL_DISABLE);
}

void Input::ShowCursor()
{
	SDL_ShowCursor(SDL_ENABLE);
}

