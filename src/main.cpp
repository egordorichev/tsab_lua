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


	return 0;
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

Uint8 *input_previous_gamepad_button_state;
Uint8 *input_current_gamepad_button_state;
Sint16 *input_gamepad_axis_state;
Uint8 *input_previous_mouse_state;
Uint8 *input_current_mouse_state;
Uint8 *input_previous_keyboard_state;
const Uint8 *input_current_keyboard_state;
SDL_GameController *controller = nullptr;

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

	input_keyboard_map["controller_x"] = SDL_CONTROLLER_BUTTON_X;
	input_keyboard_map["controller_y"] = SDL_CONTROLLER_BUTTON_Y;
	input_keyboard_map["controller_a"] = SDL_CONTROLLER_BUTTON_A;
	input_keyboard_map["controller_b"] = SDL_CONTROLLER_BUTTON_B;
	input_keyboard_map["controller_back"] = SDL_CONTROLLER_BUTTON_BACK;
	input_keyboard_map["controller_start"] = SDL_CONTROLLER_BUTTON_START;
	input_keyboard_map["controller_dpad_up"] = SDL_CONTROLLER_BUTTON_DPAD_UP;
	input_keyboard_map["controller_dpad_down"] = SDL_CONTROLLER_BUTTON_DPAD_DOWN;
	input_keyboard_map["controller_dpad_left"] = SDL_CONTROLLER_BUTTON_DPAD_LEFT;
	input_keyboard_map["controller_dpad_right"] = SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
	input_keyboard_map["controller_lstick_button"] = SDL_CONTROLLER_BUTTON_LEFTSTICK;
	input_keyboard_map["controller_rstick_button"] = SDL_CONTROLLER_BUTTON_LEFTSTICK;
	input_keyboard_map["controller_lshoulder"] = SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
	input_keyboard_map["controller_rshoulder"] = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;

	input_keyboard_map["lx"] = SDL_CONTROLLER_AXIS_LEFTX;
	input_keyboard_map["ry"] = SDL_CONTROLLER_AXIS_LEFTY;
	input_keyboard_map["lx"] = SDL_CONTROLLER_AXIS_RIGHTX;
	input_keyboard_map["ry"] = SDL_CONTROLLER_AXIS_RIGHTY;
	input_keyboard_map["ltrigger"] = SDL_CONTROLLER_AXIS_TRIGGERLEFT;
	input_keyboard_map["rtrigger"] = SDL_CONTROLLER_AXIS_TRIGGERRIGHT;
}

static int tsab_input_get_axis(lua_State *L) {
	if (controller == nullptr) {
		lua_pushboolean(L, 0); // No controller
		return 1;
	}

	const char *axis = luaL_checkstring(L, 1);
	auto it = input_keyboard_map.find(axis);

	if (it != input_keyboard_map.end()) {
		lua_pushnumber(L, std::max<double>(-1, ((double) input_gamepad_axis_state[it->second]) / 32767));
	} else {
		std::cout << "No such axis " << axis << std::endl;
		lua_pushboolean(L, 0); // Axis not found
	}

	return 1;
}

static int tsab_input_get_mouse_position(lua_State *L) {
	int x = 0;
	int y = 0;

	SDL_GetMouseState(&x, &y);

	lua_pushnumber(L, x);
	lua_pushnumber(L, y);

	return 2;
}

static int tsab_input_was_released(lua_State *L) {
	const char *key = luaL_checkstring(L, 1);

	if (strstr(key, "controller") != nullptr) {
		if (controller == nullptr) {
			lua_pushboolean(L, 0); // No controller
			return 1;
		}

		auto it = input_keyboard_map.find(key);

		if (it != input_keyboard_map.end()) {
			int scancode = it->second;

			if (input_current_gamepad_button_state[scancode] == 0 && input_previous_gamepad_button_state[scancode] == 1) {
				lua_pushboolean(L, 1);
			} else {
				lua_pushboolean(L, 0);
			}
		} else {
			std::cout << "No such control " << key << std::endl;
			lua_pushboolean(L, 0); // Control not found
		}
	} else if (strstr(key, "mouse") != nullptr) {
		auto it = input_keyboard_map.find(key);

		if (it != input_keyboard_map.end()) {
			int scancode = it->second;

			if (input_current_mouse_state[scancode] == 0 && input_previous_mouse_state[scancode] == 1) {
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

			if (input_current_keyboard_state[scancode] == 0 && input_previous_keyboard_state[scancode] == 1) {
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

static int tsab_input_is_down(lua_State *L) {
	const char *key = luaL_checkstring(L, 1);

	if (strstr(key, "controller") != nullptr) {
		if (controller == nullptr) {
			lua_pushboolean(L, 0); // No controller
			return 1;
		}

		auto it = input_keyboard_map.find(key);

		if (it != input_keyboard_map.end()) {
			int scancode = it->second;

			if (input_current_gamepad_button_state[scancode] == 1) {
				lua_pushboolean(L, 1);
			} else {
				lua_pushboolean(L, 0);
			}
		} else {
			std::cout << "No such control " << key << std::endl;
			lua_pushboolean(L, 0); // Control not found
		}
	} else if (strstr(key, "mouse") != nullptr) {
		auto it = input_keyboard_map.find(key);

		if (it != input_keyboard_map.end()) {
			int scancode = it->second;

			if (input_current_mouse_state[scancode] == 1) {
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

			if (input_current_keyboard_state[scancode] == 1) {
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

static int tsab_input_was_pressed(lua_State *L) {
	const char *key = luaL_checkstring(L, 1);

	if (strstr(key, "controller") != nullptr) {
		if (controller == nullptr) {
			lua_pushboolean(L, 0); // No controller
			return 1;
		}

		auto it = input_keyboard_map.find(key);

		if (it != input_keyboard_map.end()) {
			int scancode = it->second;

			if (input_current_gamepad_button_state[scancode] == 1 && input_previous_gamepad_button_state[scancode] == 0) {
				lua_pushboolean(L, 1);
			} else {
				lua_pushboolean(L, 0);
			}
		} else {
			std::cout << "No such control " << key << std::endl;
			lua_pushboolean(L, 0); // Control not found
		}
	} else if (strstr(key, "mouse") != nullptr) {
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

static GPU_Target *screen;
GPU_Image *current_target = nullptr;

static int tsab_graphics_get_default_canvas(lua_State *L) {
	lua_pushlightuserdata(L, screen);
	lua_pushnumber(L, screen->w);
	lua_pushnumber(L, screen->h);

	return 3;
}

static int tsab_graphics_new_canvas(lua_State *L) {
	double w = luaL_checknumber(L, 1);
	double h = luaL_checknumber(L, 2);

	GPU_Image *image = GPU_CreateImage(w, h, GPU_FORMAT_RGBA);
	GPU_SetImageFilter(image, GPU_FILTER_NEAREST);
	GPU_LoadTarget(image);
	lua_pushlightuserdata(L, image);

	return 1;
}

static int tsab_graphics_set_canvas(lua_State *L) {
	current_target = (GPU_Image *) lua_touserdata(L, 1);
	return 0;
}

static SDL_Color current_color = { 255, 255, 255, 255 };

static int tsab_graphics_draw(lua_State *L) {
	auto *target = (GPU_Target *) lua_touserdata(L, 1);
	auto *what = (GPU_Image *) lua_touserdata(L, 2);

	double x = luaL_checknumber(L, 3);
	double y = luaL_checknumber(L, 4);
	double a = luaL_checknumber(L, 5);
	double ox = luaL_checknumber(L, 6);
	double oy = luaL_checknumber(L, 7);
	double sx = luaL_checknumber(L, 8);
	double sy = luaL_checknumber(L, 9);
	double src_x = luaL_checknumber(L, 10);
	double src_y = luaL_checknumber(L, 11);
	double src_w = luaL_checknumber(L, 12);
	double src_h = luaL_checknumber(L, 13);

	GPU_Rect r = GPU_MakeRect(src_x, src_y, src_w, src_h);
	GPU_BlitTransformX(what, &r, target, x, y, ox, oy, a, sx, sy);

	return 0;
}

static int tsab_graphics_draw_to_texture(lua_State *L) {
	auto *target = (GPU_Image *) lua_touserdata(L, 1);
	auto *what = (GPU_Image *) lua_touserdata(L, 2);

	double x = luaL_checknumber(L, 3);
	double y = luaL_checknumber(L, 4);
	double a = luaL_checknumber(L, 5);
	double ox = luaL_checknumber(L, 6);
	double oy = luaL_checknumber(L, 7);
	double sx = luaL_checknumber(L, 8);
	double sy = luaL_checknumber(L, 9);
	double src_x = luaL_checknumber(L, 10);
	double src_y = luaL_checknumber(L, 11);
	double src_w = luaL_checknumber(L, 12);
	double src_h = luaL_checknumber(L, 13);

	GPU_Rect r = GPU_MakeRect(src_x, src_y, src_w, src_h);
	GPU_BlitTransformX(what, &r, target->target, x, y, ox, oy, a, sx, sy);

	return 0;
}

static bool check_bool(lua_State *L, int index, bool default_bool) {
  if (lua_isboolean( L, index)) {
    return static_cast<bool>(lua_toboolean(L, index));
  }

  return default_bool;
}

static double check_number(lua_State *L, int index, double default_number) {
	if (lua_isnumber(L, index)) {
		return lua_tonumber(L, index);
	}

	return default_number;
}

static int tsab_graphics_circle(lua_State *L) {
	double x = luaL_checknumber(L, 1);
	double y = luaL_checknumber(L, 2);
	double r = luaL_checknumber(L, 3);
	bool filled = check_bool(L, 4, true);

	if (filled) {
		GPU_CircleFilled(current_target == nullptr ? screen : current_target->target, x, y, r, current_color);
	} else {
		GPU_Circle(current_target == nullptr ? screen : current_target->target, x, y, r, current_color);
	}

	return 0;
}

static int tsab_graphics_ellipse(lua_State *L) {
	double x = luaL_checknumber(L, 1);
	double y = luaL_checknumber(L, 2);
	double w = luaL_checknumber(L, 3);
	double h = luaL_checknumber(L, 4);
	double d = check_number(L, 5, 0);
	bool filled = check_bool(L, 6, true);

	if (filled) {
		GPU_EllipseFilled(current_target == nullptr ? screen : current_target->target, x, y, w, h, d, current_color);
	} else {
		GPU_Ellipse(current_target == nullptr ? screen : current_target->target, x, y, w, h, d, current_color);
	}

	return 0;
}

static int tsab_graphics_rectangle(lua_State *L) {
	double x = luaL_checknumber(L, 1);
	double y = luaL_checknumber(L, 2);
	double w = luaL_checknumber(L, 3);
	double h = luaL_checknumber(L, 4);
	bool filled = check_bool(L, 5, true);

	if (filled) {
		GPU_RectangleFilled(current_target == nullptr ? screen : current_target->target, x, y, x + w, y + h, current_color);
	} else {
		GPU_Rectangle(current_target == nullptr ? screen : current_target->target, x, y, x + w, y + h, current_color);
	}

	return 0;
}

static int tsab_graphics_triangle(lua_State *L) {
	double x1 = luaL_checknumber(L, 1);
	double y1 = luaL_checknumber(L, 2);
	double x2 = luaL_checknumber(L, 3);
	double y2 = luaL_checknumber(L, 4);
	double x3 = luaL_checknumber(L, 5);
	double y3 = luaL_checknumber(L, 6);

	bool filled = check_bool(L, 7, true);

	if (filled) {
		GPU_TriFilled(current_target == nullptr ? screen : current_target->target, x1, y1, x2, y2, x3, y3, current_color);
	} else {
		GPU_Tri(current_target == nullptr ? screen : current_target->target, x1, y1, x2, y2, x3, y3, current_color);
	}

	return 0;
}

static int tsab_graphics_point(lua_State *L) {
	double x = luaL_checknumber(L, 1);
	double y = luaL_checknumber(L, 2);

	GPU_Pixel(current_target == nullptr ? screen : current_target->target, x + 0.5, y + 0.5, current_color);

	return 0;
}

static int tsab_graphics_line(lua_State *L) {
	double x1 = luaL_checknumber(L, 1);
	double y1 = luaL_checknumber(L, 2);
	double x2 = luaL_checknumber(L, 3);
	double y2 = luaL_checknumber(L, 4);

	GPU_Line(current_target == nullptr ? screen : current_target->target, x1 + 0.5, y1 + 0.5, x2 + 0.5, y2 + 0.5, current_color);

	return 0;
}

static int clamp(int val, int min, int max) {
	return std::min(std::max(min, val), max);
}

static int tsab_graphics_set_color(lua_State *L) {
	double r = check_number(L, 1, 1);
	double g = check_number(L, 2, 1);
	double b = check_number(L, 3, 1);
	double a = check_number(L, 4, 1);

	current_color.r = clamp(r * 255, 0, 255);
	current_color.g = clamp(g * 255, 0, 255);
	current_color.b = clamp(b * 255, 0, 255);
	current_color.a = clamp(a * 255, 0, 255);

	return 0;
}

static int tsab_graphics_get_color(lua_State *L) {
	lua_pushnumber(L, ((double) current_color.r) / 255);
	lua_pushnumber(L, ((double) current_color.g) / 255);
	lua_pushnumber(L, ((double) current_color.b) / 255);
	lua_pushnumber(L, ((double) current_color.a) / 255);

	return 4;
}

int main(int, char **) {
	GPU_SetDebugLevel(GPU_DEBUG_LEVEL_MAX);
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");

	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	// Register API
	lua_pushcfunction(L, traceback);
	// Main API
	lua_register(L, "tsab_quit", tsab_quit);
	// Input API
	lua_register(L, "tsab_input_get_mouse_position", tsab_input_get_mouse_position);
	lua_register(L, "tsab_input_is_down", tsab_input_is_down);
	lua_register(L, "tsab_input_was_released", tsab_input_was_released);
	lua_register(L, "tsab_input_was_pressed", tsab_input_was_pressed);
	lua_register(L, "tsab_input_get_axis", tsab_input_get_axis);
	// Graphics API
	lua_register(L, "tsab_graphics_get_default_canvas", tsab_graphics_get_default_canvas);
	lua_register(L, "tsab_graphics_new_canvas", tsab_graphics_new_canvas);
	lua_register(L, "tsab_graphics_set_canvas", tsab_graphics_set_canvas);
	lua_register(L, "tsab_graphics_set_color", tsab_graphics_set_color);
	lua_register(L, "tsab_graphics_get_color", tsab_graphics_get_color);
	lua_register(L, "tsab_graphics_draw", tsab_graphics_draw);
	lua_register(L, "tsab_graphics_draw_to_texture", tsab_graphics_draw_to_texture);
	lua_register(L, "tsab_graphics_circle", tsab_graphics_circle);
	lua_register(L, "tsab_graphics_rectangle", tsab_graphics_rectangle);
	lua_register(L, "tsab_graphics_point", tsab_graphics_point);
	lua_register(L, "tsab_graphics_line", tsab_graphics_line);
	lua_register(L, "tsab_graphics_ellipse", tsab_graphics_ellipse);
	lua_register(L, "tsab_graphics_triangle", tsab_graphics_triangle);

	// Create window
	screen = GPU_Init(window_width, window_height, pack_window_flags());
	GPU_Clear(screen);

	// Do the api file
	int rv = luaL_dofile(L, "api.lua");

	if (rv) {
		std::cerr << lua_tostring(L, -1) << std::endl;
		return rv;
	}

	// Do the main files
	rv = luaL_loadfile(L, "main.lua");

	if (rv) {
		std::cerr << lua_tostring(L, -1) << std::endl;
		return rv;
	} else {
		lua_pcall(L, 0, 0, lua_gettop(L) - 1);
	}

	// Init key map
	setup_key_map();
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

	input_previous_gamepad_button_state = new Uint8[SDL_CONTROLLER_BUTTON_MAX - SDL_CONTROLLER_BUTTON_INVALID + 1];
	input_current_gamepad_button_state = new Uint8[SDL_CONTROLLER_BUTTON_MAX - SDL_CONTROLLER_BUTTON_INVALID + 1];
	input_gamepad_axis_state = new Sint16[SDL_CONTROLLER_AXIS_MAX - SDL_CONTROLLER_AXIS_INVALID + 1];

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

		for (int i = 0; i < 24; i++) {
			input_previous_gamepad_button_state[i] = input_current_gamepad_button_state[i];
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
				case SDL_JOYDEVICEADDED:
				case SDL_CONTROLLERDEVICEADDED:
					if (controller == nullptr) {
						controller = SDL_GameControllerOpen(event.cdevice.which);

						if (controller == nullptr) {
							std::cerr << "Failed to connect controller: " << SDL_GetError() << "\n";
						} else {
							std::cout << "Registered controller\n";
						}
					}
					break;
				case SDL_JOYDEVICEREMOVED:
				case SDL_CONTROLLERDEVICEREMOVED:
					if (controller == reinterpret_cast<SDL_GameController *>(event.cdevice.which)) {
						controller = nullptr;
						std::cout << "Removed controller\n";
					}
					break;
				default: break;
			}
		}

		if (controller != nullptr) {
			for (int i = SDL_CONTROLLER_BUTTON_INVALID; i < SDL_CONTROLLER_BUTTON_MAX; i++) {
				input_current_gamepad_button_state[i] = SDL_GameControllerGetButton(controller, (SDL_GameControllerButton) i);
			}

			for (int i = SDL_CONTROLLER_AXIS_INVALID; i < SDL_CONTROLLER_AXIS_MAX; i++) {
				input_gamepad_axis_state[i] = SDL_GameControllerGetAxis(controller, (SDL_GameControllerAxis) i);
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