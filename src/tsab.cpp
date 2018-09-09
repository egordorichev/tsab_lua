/*
 * tsab
 * by @egordorichev
 *
 * TODO:
 *  + Fix fps
 */

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_GPU/SDL_gpu.h>

#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <vector>
#include <sstream>

#include "tsab.hpp"
#include "audio.hpp"
#include "input.hpp"
#include "fs.hpp"
#include "shaders.hpp"
#include "graphics.hpp"
#include "physics.hpp"
#include "lua.hpp"

/*
 * Lua stuff
 */

static int traceback(lua_State *L) {
	lua_pushglobaltable(L);
	lua_getfield(L, -1, "debug");
	lua_remove(L, -2);
	lua_getfield(L, -1, "traceback");
	lua_pushvalue(L, 1);
	lua_pushinteger(L, 2);
	lua_call(L, 2, 1);

	return 1;
}

bool running = true;
int fps;

static int tsab_quit(lua_State *L) {
	running = false;
	return 0;
}

static int tsab_get_time(lua_State *L) {
	lua_pushnumber(L, ((double) SDL_GetTicks()) / 1000);
	return 1;
}

static int tsab_get_fps(lua_State *L) {
	lua_pushnumber(L, fps);
	return 1;
}

static void report_lua_error(lua_State *L) {
	const char *error = lua_tostring(L, -1);

	lua_getglobal(L, "tsab_error");
	lua_pushstring(L, error);
	lua_pcall(L, 1, 0, 0);
}

/*
 * Main stuff
 */

lua_State *L;

int tsab_init(int arg, char **argv) {
	if (arg >= 2) {
		tsab_fs_set_working_dir(argv[1]);
	}

	// Do the config file
	L = luaL_newstate();
	luaL_openlibs(L);

	lua_pushcfunction(L, traceback);
	int rv = luaL_loadfile(L, "config.lua");

	if (rv) {
		std::cerr << lua_tostring(L, -1) << std::endl;
		return rv;
	} else {
		if (lua_pcall(L, 0, 0, lua_gettop(L) - 1) != 0) {
			report_lua_error(L);
		}
	}

	lua_getglobal(L, "config");
	lua_pushstring(L, "window");
	lua_gettable(L, -2);

	int window_width = 640;
	int window_height = 480;
	int window_min_width = 0;
	int window_min_height = 0;
	bool resizeable;
	char *window_title;

	if (!lua_isnil(L, -1)) {
		lua_pushstring(L, "width");
		lua_gettable(L, -2);
		window_width = check_number(L, -1, window_width);
		lua_pop(L, 1);

		lua_pushstring(L, "height");
		lua_gettable(L, -2);
		window_height = check_number(L, -1, window_height);
		lua_pop(L, 1);

		lua_pushstring(L, "min_width");
		lua_gettable(L, -2);
		window_min_width = check_number(L, -1, window_min_width);
		lua_pop(L, 1);

		lua_pushstring(L, "min_height");
		lua_gettable(L, -2);
		window_min_height = check_number(L, -1, window_min_height);
		lua_pop(L, 1);

		lua_pushstring(L, "resizeable");
		lua_gettable(L, -2);
		resizeable = check_bool(L, -1, true);
		lua_pop(L, 1);

		lua_pushstring(L, "title");
		lua_gettable(L, -2);
		window_title = (char *) check_string(L, -1, "To Steam and beyond!");
		lua_pop(L, 1);
	}

	lua_pop(L, 1); // Window level
	lua_pop(L, 1);

	int flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;

	if (resizeable) {
		flags |= SDL_WINDOW_RESIZABLE;
	}

	tsab_graphics_init(window_width, window_height, window_min_width, window_min_height, window_title, flags);
	tsab_audio_init();
	tsab_input_init();

	// Register API
	lua_register(L, "tsab_get_time", tsab_get_time);
	lua_register(L, "tsab_get_fps", tsab_get_fps);
	lua_register(L, "tsab_quit", tsab_quit);
	
	tsab_input_register_api(L);
	tsab_graphics_register_api(L);
	tsab_shaders_register_api(L);
	tsab_audio_register_api(L);
	tsab_fs_register_api(L);
	tsab_physics_register_api(L);

	// Do the api file
	lua_pushcfunction(L, traceback);
	rv = luaL_loadfile(L, "api.lua");

	if (rv) {
		std::cerr << lua_tostring(L, -1) << std::endl;
		return rv;
	} else {
		if (lua_pcall(L, 0, 0, lua_gettop(L) - 1) != 0) {
			report_lua_error(L);
		}
	}

	// Do the main file
	lua_pushcfunction(L, traceback);
	rv = luaL_loadfile(L, "main.lua");

	if (rv) {
		std::cerr << lua_tostring(L, -1) << std::endl;
		return rv;
	} else {
		if (lua_pcall(L, 0, 0, lua_gettop(L) - 1) != 0) {
			report_lua_error(L);
		}
	}

	// Call init
	lua_pushcfunction(L, traceback);
	lua_getglobal(L, "tsab_init");

	if (lua_pcall(L, 0, 0, lua_gettop(L) - 1) != 0) {
		report_lua_error(L);
	}

	return 0;
}

Uint64 timer_now = SDL_GetPerformanceCounter();
Uint64 timer_last = 0;
double timer_dt = 0;
double timer_accumulator = 0;
int frame;
Uint32 start;
int w;
int h;
SDL_Event event;

void tsab_loop() {
	tsab_graphics_get_ready();

	while (running) {
		tsab_loop_step();
	}
}

bool tsab_loop_step() {
	start = SDL_GetTicks();
	tsab_input_update();
	
	while (SDL_PollEvent(&event)) {
		tsab_input_handle_event(&event);
		
		switch (event.type) {
			case SDL_QUIT:
				running = false;
				break;
			case SDL_WINDOWEVENT:
				switch (event.window.event) {
					case SDL_WINDOWEVENT_RESIZED:
						lua_pushcfunction(L, traceback);
						lua_getglobal(L, "tsab_resize");

						tsab_graphics_resize(&w, &h);

						lua_pushnumber(L, w);
						lua_pushnumber(L, h);

						if (lua_pcall(L, 2, 0, lua_gettop(L) - 3) != 0) {
							report_lua_error(L);
						}

						break;
				}
				break;
		}
	}

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

	tsab_graphics_clear_screen();

	// Call draw
	lua_pushcfunction(L, traceback);
	lua_getglobal(L, "tsab_draw");

	if (lua_pcall(L, 0, 0, lua_gettop(L) - 1) != 0) {
		report_lua_error(L);
	}

	tsab_graphics_flip(NULL);
	SDL_Delay(1);

	timer_last = timer_now;
	timer_now = SDL_GetPerformanceCounter();
	timer_dt = ((timer_now - timer_last) / (double) SDL_GetPerformanceFrequency());

	if (frame % 40 == 0) {
		fps = 1000.0 / ((double) (SDL_GetTicks() - start));
	}

	frame ++;
	return !running;
}

void tsab_quit() {
	tsab_shaders_quit();
	tsab_audio_quit();
	tsab_input_quit();
	tsab_graphics_quit();

	// Call destroy
	lua_pushcfunction(L, traceback);
	lua_getglobal(L, "tsab_destroy");

	if (lua_pcall(L, 0, 0, lua_gettop(L) - 1) != 0) {
		report_lua_error(L);
	}
}