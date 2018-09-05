#include <SDL.h>
#include <SDL_GPU/SDL_gpu.h>
#include <LuaJIT/lua.hpp>

#include <iostream>
#include <string>
#include <cstring>
#include <map>

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

const Uint8 *input_previous_keyboard_state;
const Uint8 *input_current_keyboard_state;
static std::map<std::string, SDL_Keycode> input_keyboard_map;

static int tsab_input_was_pressed(lua_State *L) {
	const char *key = luaL_checkstring(L, 1);
	SDL_Keycode keycode = input_keyboard_map[key];

	if (keycode) {
		SDL_Scancode scancode = SDL_GetScancodeFromKey(keycode);

		if (input_current_keyboard_state[scancode] && !input_previous_keyboard_state[scancode]) {
			lua_pushboolean(L, 1);
		} else {
			lua_pushboolean(L, 0);
		}
	}

	return 1;
}

int main(int, char **){
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
	input_keyboard_map["q"] = SDL_SCANCODE_Q;

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
	input_previous_keyboard_state = SDL_GetKeyboardState(nullptr);
	input_current_keyboard_state = SDL_GetKeyboardState(nullptr);

	// Timer vars
	Uint64 timer_now = SDL_GetPerformanceCounter();
	Uint64 timer_last = 0;
	double timer_dt = 0;
	double timer_fixed_dt = 1.0 / 60.0;
	double timer_accumulator = 0;

	while (running) {
		// Handle input
		memcpy(&input_previous_keyboard_state, input_current_keyboard_state, 512);

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT: running = false; break;
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

	GPU_Quit();

	// Call destroy
	lua_pushcfunction(L, traceback);
	lua_getglobal(L, "tsab_destroy");

	if (lua_pcall(L, 0, 0, lua_gettop(L) - 1) != 0) {
		report_lua_error(L);
	}

	return 0;
}