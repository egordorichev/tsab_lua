#include <SDL.h>
#include <SDL_GPU/SDL_gpu.h>
#include <LuaJIT/lua.hpp>

#include <iostream>
#include <string>
#include <cstring>
#include <map>

#define MOUSE_1 0
#define MOUSE_2 1
#define MOUSE_3 2
#define MOUSE_4 3
#define MOUSE_5 4
#define MOUSE_WHEEL_UP 5
#define MOUSE_WHEEL_DOWN 6
#define MOUSE_WHEEL_LEFT 7
#define MOUSE_WHEEL_RIGHT 8

static bool running = true;

/*
 * Window config
 */

static int window_width = 640;
static int window_height = 480;
static bool resizeable = true;

static Uint32 pack_window_flags() {
	Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;

	if (resizeable) {
		flags |= SDL_WINDOW_RESIZABLE;
	}

	return flags;
}

/*
 * Lua stuff
 */

static int traceback(lua_State *L) {
	lua_getfield(L, LUA_GLOBALSINDEX, "debug");
	lua_getfield(L, -1, "traceback");
	lua_pushvalue(L, 1);
	lua_pushinteger(L, 2);
	lua_call(L, 2, 1);

	return 1;
}

static int tsab_quit(lua_State *L) {
	running = false;
	return 0;
}

static void report_lua_error(lua_State *L) {
	const char *error = lua_tostring(L, -1);

	lua_getglobal(L, "tsab_error");
	lua_pushstring(L, error);
	lua_pcall(L, 1, 0, 0);
}

Uint8 *input_previous_mouse_state;
Uint8 *input_current_mouse_state;
Uint8 *input_previous_keyboard_state;
const Uint8 *input_current_keyboard_state;

// Compares two strings in a map
struct compare
{
		bool operator() (char const *a, char const *b) {
			return std::strcmp(a, b) < 0;
		}
};

static std::map<const char *, int, compare> input_keyboard_map;

static void setup_key_map() {
	input_keyboard_map["a"] = SDL_SCANCODE_A;
	input_keyboard_map["b"] = SDL_SCANCODE_B;
	input_keyboard_map["c"] = SDL_SCANCODE_C;
	input_keyboard_map["d"] = SDL_SCANCODE_D;
	input_keyboard_map["e"] = SDL_SCANCODE_E;
	input_keyboard_map["f"] = SDL_SCANCODE_F;
	input_keyboard_map["g"] = SDL_SCANCODE_G;
	input_keyboard_map["h"] = SDL_SCANCODE_H;
	input_keyboard_map["i"] = SDL_SCANCODE_I;
	input_keyboard_map["j"] = SDL_SCANCODE_J;
	input_keyboard_map["k"] = SDL_SCANCODE_K;
	input_keyboard_map["l"] = SDL_SCANCODE_L;
	input_keyboard_map["m"] = SDL_SCANCODE_M;
	input_keyboard_map["n"] = SDL_SCANCODE_N;
	input_keyboard_map["o"] = SDL_SCANCODE_O;
	input_keyboard_map["p"] = SDL_SCANCODE_P;
	input_keyboard_map["q"] = SDL_SCANCODE_Q;
	input_keyboard_map["r"] = SDL_SCANCODE_R;
	input_keyboard_map["s"] = SDL_SCANCODE_S;
	input_keyboard_map["t"] = SDL_SCANCODE_T;
	input_keyboard_map["u"] = SDL_SCANCODE_U;
	input_keyboard_map["v"] = SDL_SCANCODE_V;
	input_keyboard_map["w"] = SDL_SCANCODE_W;
	input_keyboard_map["x"] = SDL_SCANCODE_X;
	input_keyboard_map["y"] = SDL_SCANCODE_Y;
	input_keyboard_map["z"] = SDL_SCANCODE_Z;
	input_keyboard_map["0"] = SDL_SCANCODE_0;
	input_keyboard_map["1"] = SDL_SCANCODE_1;
	input_keyboard_map["2"] = SDL_SCANCODE_2;
	input_keyboard_map["3"] = SDL_SCANCODE_3;
	input_keyboard_map["4"] = SDL_SCANCODE_4;
	input_keyboard_map["5"] = SDL_SCANCODE_5;
	input_keyboard_map["6"] = SDL_SCANCODE_6;
	input_keyboard_map["7"] = SDL_SCANCODE_7;
	input_keyboard_map["8"] = SDL_SCANCODE_8;
	input_keyboard_map["9"] = SDL_SCANCODE_9;
	input_keyboard_map["f0"] = SDL_SCANCODE_F1;
	input_keyboard_map["f1"] = SDL_SCANCODE_F2;
	input_keyboard_map["f2"] = SDL_SCANCODE_F2;
	input_keyboard_map["f3"] = SDL_SCANCODE_F3;
	input_keyboard_map["f4"] = SDL_SCANCODE_F4;
	input_keyboard_map["f5"] = SDL_SCANCODE_F5;
	input_keyboard_map["f6"] = SDL_SCANCODE_F6;
	input_keyboard_map["f7"] = SDL_SCANCODE_F7;
	input_keyboard_map["f8"] = SDL_SCANCODE_F8;
	input_keyboard_map["f9"] = SDL_SCANCODE_F9;
	input_keyboard_map["f10"] = SDL_SCANCODE_F10;
	input_keyboard_map["f11"] = SDL_SCANCODE_F11;
	input_keyboard_map["f12"] = SDL_SCANCODE_F12;
	input_keyboard_map["space"] = SDL_SCANCODE_SPACE;
	input_keyboard_map["lshift"] = SDL_SCANCODE_LSHIFT;
	input_keyboard_map["rshift"] = SDL_SCANCODE_RSHIFT;
	input_keyboard_map["rcontrol"] = SDL_SCANCODE_LCTRL;
	input_keyboard_map["lcontrol"] = SDL_SCANCODE_RCTRL;
	input_keyboard_map["lalt"] = SDL_SCANCODE_LALT;
	input_keyboard_map["ralt"] = SDL_SCANCODE_RALT;
	input_keyboard_map["capslock"] = SDL_SCANCODE_CAPSLOCK;
	input_keyboard_map["tab"] = SDL_SCANCODE_TAB;
	input_keyboard_map["escape"] = SDL_SCANCODE_ESCAPE;
	input_keyboard_map["delete"] = SDL_SCANCODE_DELETE;
	input_keyboard_map["backspace"] = SDL_SCANCODE_BACKSPACE;
	input_keyboard_map["slash"] = SDL_SCANCODE_SLASH;
	input_keyboard_map["bslash"] = SDL_SCANCODE_BACKSLASH;
	input_keyboard_map["home"] = SDL_SCANCODE_HOME;
	input_keyboard_map["end"] = SDL_SCANCODE_END;
	input_keyboard_map["page_up"] = SDL_SCANCODE_PAGEUP;
	input_keyboard_map["page_down"] = SDL_SCANCODE_PAGEDOWN;
	input_keyboard_map["left"] = SDL_SCANCODE_LEFT;
	input_keyboard_map["right"] = SDL_SCANCODE_RIGHT;
	input_keyboard_map["up"] = SDL_SCANCODE_UP;
	input_keyboard_map["down"] = SDL_SCANCODE_DOWN;
	input_keyboard_map["comma"] = SDL_SCANCODE_COMMA;

	input_keyboard_map["mouse1"] = MOUSE_1;
	input_keyboard_map["mouse2"] = MOUSE_2;
	input_keyboard_map["mouse3"] = MOUSE_3;
	input_keyboard_map["mouse4"] = MOUSE_4;
	input_keyboard_map["mouse5"] = MOUSE_5;
	input_keyboard_map["mouse_wheel_up"] = MOUSE_WHEEL_UP;
	input_keyboard_map["mouse_wheel_down"] = MOUSE_WHEEL_DOWN;
	input_keyboard_map["mouse_wheel_left"] = MOUSE_WHEEL_LEFT;
	input_keyboard_map["mouse_wheel_right"] = MOUSE_WHEEL_RIGHT;
}

static int tsab_input_was_pressed(lua_State *L) {
	const char *key = luaL_checkstring(L, 1);

	if (strstr(key, "mouse") != nullptr) {
		auto it = input_keyboard_map.find(key);

		if (it != input_keyboard_map.end()) {
			int scancode = it->second;

			if (input_current_mouse_state[scancode] == 1 && input_previous_mouse_state[scancode] == 0) {
				lua_pushboolean(L, 1);
			} else {
				lua_pushboolean(L, 0);
			}
		} else {
			std::cout << "No such mouse control " << key << std::endl;
			lua_pushboolean(L, 0); // Control not found
		}
	} else {
		auto it = input_keyboard_map.find(key);

		if (it != input_keyboard_map.end()) {
			int scancode = it->second;

			if (input_current_keyboard_state[scancode] == 1 && input_previous_keyboard_state[scancode] == 0) {
				lua_pushboolean(L, 1);
			} else {
				lua_pushboolean(L, 0);
			}
		} else {
			std::cout << "No such key " << key << std::endl;
			lua_pushboolean(L, 0); // Key not found
		}
	}

	return 1;
}

int main(int, char **) {
	GPU_SetDebugLevel(GPU_DEBUG_LEVEL_MAX);
	SDL_Init(SDL_INIT_EVERYTHING);

	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	int rv = luaL_loadfile(L, "main.lua");

	if (rv) {
		std::cerr << lua_tostring(L, -1) << std::endl;
		return rv;
	} else {
		lua_pcall(L, 0, 0, lua_gettop(L) - 1);
	}

	// Init key map
	setup_key_map();

	GPU_Target *screen = GPU_Init(window_width, window_height, pack_window_flags());

	// Register API
	lua_pushcfunction(L, traceback);
	lua_register(L, "tsab_quit", tsab_quit);
	lua_register(L, "tsab_input_was_pressed", tsab_input_was_pressed);

	// Call init
	lua_getglobal(L, "tsab_init");

	if (lua_pcall(L, 0, 0, lua_gettop(L) - 1) != 0) {
		report_lua_error(L);
	}

	// Event / input vars
	SDL_Event event;
	input_previous_keyboard_state = new Uint8[512];
	input_current_keyboard_state = SDL_GetKeyboardState(nullptr);

	input_previous_mouse_state = new Uint8[12];
	input_current_mouse_state = new Uint8[12];

	// Timer vars
	Uint64 timer_now = SDL_GetPerformanceCounter();
	Uint64 timer_last = 0;
	double timer_dt = 0;
	double timer_fixed_dt = 1.0 / 60.0;
	double timer_accumulator = 0;

	while (running) {
		// Handle input
		// memcpy(&input_previous_keyboard_state, input_current_keyboard_state, 512);

		// Fixme, use memcpy
		for (int i = 0; i < 512; i++) {
			input_previous_keyboard_state[i] = input_current_keyboard_state[i];
		}

		for (int i = 0; i < 12; i++) {
			input_previous_mouse_state[i] = input_current_mouse_state[i];
			input_current_mouse_state[i] = 0;
		}

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT: running = false; break;
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == SDL_BUTTON_LEFT) input_current_mouse_state[MOUSE_1] = 1;
					if (event.button.button == SDL_BUTTON_RIGHT) input_current_mouse_state[MOUSE_2] = 1;
					if (event.button.button == SDL_BUTTON_MIDDLE) input_current_mouse_state[MOUSE_3] = 1;
					if (event.button.button == SDL_BUTTON_X1) input_current_mouse_state[MOUSE_4] = 1;
					if (event.button.button == SDL_BUTTON_X2) input_current_mouse_state[MOUSE_5] = 1;
				break;
				case SDL_MOUSEWHEEL:
					if (event.wheel.x == 1) input_current_mouse_state[MOUSE_WHEEL_RIGHT] = 1;
					if (event.wheel.x == -1) input_current_mouse_state[MOUSE_WHEEL_LEFT] = 1;
					if (event.wheel.y == 1) input_current_mouse_state[MOUSE_WHEEL_DOWN] = 1;
					if (event.wheel.y == -1) input_current_mouse_state[MOUSE_WHEEL_UP] = 1;
				break;
				default: break;
			}
		}

		input_current_keyboard_state = SDL_GetKeyboardState(nullptr);

		// Handle updates
		timer_accumulator += timer_dt;

		while (timer_accumulator >= timer_fixed_dt) {
			// Call update
			lua_pushcfunction(L, traceback);
			lua_getglobal(L, "tsab_update");
			lua_pushnumber(L, timer_fixed_dt);

			if (lua_pcall(L, 1, 0, lua_gettop(L) - 2) != 0) {
				report_lua_error(L);
			}

			timer_accumulator -= timer_fixed_dt;
		}

		GPU_Clear(screen);

		// Call draw
		lua_pushcfunction(L, traceback);
		lua_getglobal(L, "tsab_draw");

		if (lua_pcall(L, 0, 0, lua_gettop(L) - 1) != 0) {
			report_lua_error(L);
		}

		GPU_Flip(screen);
		SDL_Delay(1);

		timer_last = timer_now;
		timer_now = SDL_GetPerformanceCounter();
		timer_dt = ((timer_now - timer_last) / (double) SDL_GetPerformanceFrequency());
	}

	delete input_previous_mouse_state;
	delete input_current_mouse_state;
	delete input_previous_keyboard_state;

	GPU_Quit();

	// Call destroy
	lua_pushcfunction(L, traceback);
	lua_getglobal(L, "tsab_destroy");

	if (lua_pcall(L, 0, 0, lua_gettop(L) - 1) != 0) {
		report_lua_error(L);
	}

	return 0;
}