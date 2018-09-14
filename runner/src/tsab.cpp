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
#include <11Zip/elzip.hpp>

#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <vector>
#include <sstream>
#include <experimental/filesystem>
#include <fcntl.h>

namespace fs = std::experimental::filesystem;

#include "tsab.hpp"
#include "audio.hpp"
#include "input.hpp"
#include "fs.hpp"
#include "shaders.hpp"
#include "graphics.hpp"
#include "physics.hpp"
#include "ui.hpp"
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
int fps = 60;

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

#include <chrono>
#include <thread>

int open_zip(char *path) {
	if (!fs::exists(path)) {
		std::cerr << path << " was not found\n";
		return -1;
	}

	if (fs::exists("/tmp/tsab")) {
		fs::remove_all("/tmp/tsab");
		fs::remove("/tmp/tsab");
	}

	std::string tmp = std::string(fs::temp_directory_path().generic_string().c_str()) + "/tsab";
	elz::extractZip(path, tmp);
	tsab_fs_set_working_dir(tmp.c_str());

	return 0;
}

int tsab_init(int arg, char **argv) {
	if (arg >= 2) {
		char *path = argv[1];
		char *dot = strrchr(path, '.');

		if (dot && strcmp(dot, ".tsab") == 0 || dot && strcmp(dot, ".zip") == 0) {
			if (open_zip(path) != 0) {
				return -1;
			}
		} else {
			tsab_fs_set_working_dir(path);
		}
	} else if (fs::exists(DEFAULT_CONTENT_ZIP)) {
		open_zip((char *) DEFAULT_CONTENT_ZIP);
	}

	// Do the config file
	L = luaL_newstate();
	luaL_openlibs(L);

	int window_width = 640;
	int window_height = 480;
	int window_min_width = 0;
	int window_min_height = 0;
	bool resizeable;
	bool vsync = true;
	char *window_title;
	int rv;

	if (fs::exists("config.lua")) {
		lua_pushcfunction(L, traceback);
		rv = luaL_loadfile(L, "config.lua");

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

		if (!lua_isnil(L, -1)) {
			lua_pushstring(L, "width");
			lua_gettable(L, -2);
			window_width = check_number(L, -1, window_width);
			lua_pop(L, 1);

			lua_pushstring(L, "vsync");
			lua_gettable(L, -2);
			vsync = check_bool(L, -1, vsync);
			lua_pop(L, 1);

			lua_pushstring(L, "fps");
			lua_gettable(L, -2);
			fps = check_number(L, -1, fps);
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
	} else {
		std::cout << "Warning: config.lua was not found\n";
	}

	int flags = SDL_WINDOW_OPENGL;

	if (resizeable) {
		flags |= SDL_WINDOW_RESIZABLE;
	}

	tsab_graphics_init(window_width, window_height, window_min_width, window_min_height, window_title, flags);

	SDL_GL_SetSwapInterval(0);

	tsab_audio_init();
	tsab_input_init();
	tsab_ui_init();

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
	tsab_ui_register_api(L);

	if (fs::exists("api.lua")) {
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
	} else {
		std::cout << "Warning: api.lua was not found\n";
	}

	if (fs::exists("main.lua")) {
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
	} else {
		std::cerr << "Error: main.lua was not found\n";
		tsab_quit();
		return -1;
	}

	return 0;
}

double timer_dt = 0;
int frame;
Uint32 start;
int w;
int h;
SDL_Event event;
double frame_delay;

void tsab_loop() {
	tsab_graphics_get_ready();

	frame_delay = 1000.0 / fps;
	timer_dt = frame_delay;

	while (running) {
		tsab_loop_step();
	}
}

bool tsab_loop_step() {
	start = SDL_GetTicks();
	tsab_input_update();
	
	while (SDL_PollEvent(&event)) {
		tsab_input_handle_event(&event);
		tsab_ui_handle_event(&event);

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

	// Call update
	lua_pushcfunction(L, traceback);
	lua_getglobal(L, "tsab_update");
	lua_pushnumber(L, timer_dt * 0.001);

	if (lua_pcall(L, 1, 0, lua_gettop(L) - 2) != 0) {
		report_lua_error(L);
	}

	tsab_graphics_clear_screen();

	// Call draw
	lua_pushcfunction(L, traceback);
	lua_getglobal(L, "tsab_draw");

	if (lua_pcall(L, 0, 0, lua_gettop(L) - 1) != 0) {
		report_lua_error(L);
	}

	tsab_graphics_flip(NULL);

	// FPS handling
	timer_dt = SDL_GetTicks() - start;

	if (timer_dt < frame_delay) {
		SDL_Delay(frame_delay - timer_dt);
	}

	if (frame % 40 == 0) {
		fps = 1000 / (SDL_GetTicks() - start);
	}

	frame ++;
	return !running;
}

void tsab_quit() {
	tsab_shaders_quit();
	tsab_audio_quit();
	tsab_input_quit();
	tsab_ui_quit();
	tsab_graphics_quit();

	// Call destroy
	lua_pushcfunction(L, traceback);
	lua_getglobal(L, "tsab_destroy");

	if (lua_pcall(L, 0, 0, lua_gettop(L) - 1) != 0) {
		report_lua_error(L);
	}
}