#pragma once
#ifndef INPUT_H
#define INPUT_H
#define GLM_ENABLE_EXPERIMENTAL

#include <string>
#include "ThirdParty/glm/glm.hpp"
#include "gameEngine.h"
#include "ThirdParty/glm/gtx/hash.hpp"
#include "ThirdParty/SDL2/SDL.h"
#include <vector>
#include "lua.hpp"
#include "ThirdParty/LuaBridge/LuaBridge.h"

enum INPUT_STATE {
	INPUT_STATE_UP,
	INPUT_STATE_JUST_BECAME_DOWN,
	INPUT_STATE_DOWN,
	INPUT_STATE_JUST_BECAME_UP
};
class Input
{
public:

	static void Init();
	static void ProcessEvent(const SDL_Event& e);
	static void LateUpdate();
	static bool GetKey(SDL_Scancode keycode);
	static bool GetKeyDown(SDL_Scancode keycode);
	static bool GetKeyUp(SDL_Scancode keycode);

	static bool LuaGetKey(std::string keycode);
	static bool LuaGetKeyDown(std::string keycode);
	static bool LuaGetKeyUp(std::string keycode);
	static glm::vec2 GetMousePosition();
	static bool GetMouseButton(uint8_t buttonNum);
	static bool GetMouseButtonDown(uint8_t buttonNum);
	static bool GetMouseButtonUp(uint8_t buttonNum);
	static float GetMouseScrollDelta();
	static void HideCursor();
	static void ShowCursor();


	static Input* pointerToSelf;
	Input() {
		pointerToSelf = this;  // Initialize pointerToSelf in the constructor
	}

private:
	static inline std::unordered_map<SDL_Scancode, INPUT_STATE> keyboard_states;
	static inline std::vector<SDL_Scancode> just_became_down_scancodes;
	static inline std::vector<SDL_Scancode> just_became_up_scancodes;
	
	static inline glm::vec2 mousePosition;
	static float mouseWheelDelta;

	static inline std::unordered_map<uint8_t, INPUT_STATE> mouseState;
	static inline std::vector<uint8_t> just_became_down_mouseButtons;
	static inline std::vector<uint8_t> just_became_up_mouseButtons;

	const std::unordered_map<std::string, SDL_Scancode> __keycode_to_scancode = {
		// Directional (arrow) Keys
		{"up", SDL_SCANCODE_UP},
		{"down", SDL_SCANCODE_DOWN},
		{"right", SDL_SCANCODE_RIGHT},
		{"left", SDL_SCANCODE_LEFT},

		// Misc Keys
		{"escape", SDL_SCANCODE_ESCAPE},

		// Modifier Keys
		{"lshift", SDL_SCANCODE_LSHIFT},
		{"rshift", SDL_SCANCODE_RSHIFT},
		{"lctrl", SDL_SCANCODE_LCTRL},
		{"rctrl", SDL_SCANCODE_RCTRL},
		{"lalt", SDL_SCANCODE_LALT},
		{"ralt", SDL_SCANCODE_RALT},

		// Editing Keys
		{"tab", SDL_SCANCODE_TAB},
		{"return", SDL_SCANCODE_RETURN},
		{"enter", SDL_SCANCODE_RETURN},
		{"backspace", SDL_SCANCODE_BACKSPACE},
		{"delete", SDL_SCANCODE_DELETE},
		{"insert", SDL_SCANCODE_INSERT},

		// Character Keys
		{"space", SDL_SCANCODE_SPACE},
		{"a", SDL_SCANCODE_A},
		{"b", SDL_SCANCODE_B},
		{"c", SDL_SCANCODE_C},
		{"d", SDL_SCANCODE_D},
		{"e", SDL_SCANCODE_E},
		{"f", SDL_SCANCODE_F},
		{"g", SDL_SCANCODE_G},
		{"h", SDL_SCANCODE_H},
		{"i", SDL_SCANCODE_I},
		{"j", SDL_SCANCODE_J},
		{"k", SDL_SCANCODE_K},
		{"l", SDL_SCANCODE_L},
		{"m", SDL_SCANCODE_M},
		{"n", SDL_SCANCODE_N},
		{"o", SDL_SCANCODE_O},
		{"p", SDL_SCANCODE_P},
		{"q", SDL_SCANCODE_Q},
		{"r", SDL_SCANCODE_R},
		{"s", SDL_SCANCODE_S},
		{"t", SDL_SCANCODE_T},
		{"u", SDL_SCANCODE_U},
		{"v", SDL_SCANCODE_V},
		{"w", SDL_SCANCODE_W},
		{"x", SDL_SCANCODE_X},
		{"y", SDL_SCANCODE_Y},
		{"z", SDL_SCANCODE_Z},
		{"0", SDL_SCANCODE_0},
		{"1", SDL_SCANCODE_1},
		{"2", SDL_SCANCODE_2},
		{"3", SDL_SCANCODE_3},
		{"4", SDL_SCANCODE_4},
		{"5", SDL_SCANCODE_5},
		{"6", SDL_SCANCODE_6},
		{"7", SDL_SCANCODE_7},
		{"8", SDL_SCANCODE_8},
		{"9", SDL_SCANCODE_9},
		{"/", SDL_SCANCODE_SLASH},
		{";", SDL_SCANCODE_SEMICOLON},
		{"=", SDL_SCANCODE_EQUALS},
		{"-", SDL_SCANCODE_MINUS},
		{".", SDL_SCANCODE_PERIOD},
		{",", SDL_SCANCODE_COMMA},
		{"[", SDL_SCANCODE_LEFTBRACKET},
		{"]", SDL_SCANCODE_RIGHTBRACKET},
		{"\\", SDL_SCANCODE_BACKSLASH},
		{"'", SDL_SCANCODE_APOSTROPHE}
	};

};

#endif