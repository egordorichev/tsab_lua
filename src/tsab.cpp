/*
 * tsab
 * by @egordorichev
 *
 * TODO:
 *  + Fix fps
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <SDL_GPU/SDL_gpu.h>
#include <LuaJIT/lua.hpp>

#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <vector>
#include <sstream>

#include "tsab.hpp"

#ifdef _WIN32
#include <direct.h>
#define cwd _getcwd
#define cd _chdir
#else
#include "unistd.h"
#define cwd getcwd
#define cd chdir
#endif

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
static std::string working_dir = "./";
static int fps;

static std::vector<GPU_ShaderBlock> shader_blocks;
static std::vector<Uint32> shaders;

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
static SDL_Window *window;
GPU_Image *current_target = nullptr;

static int tsab_graphics_get_default_canvas(lua_State *L) {
	lua_pushnumber(L, -1);
	lua_pushnumber(L, screen->w);
	lua_pushnumber(L, screen->h);

	return 3;
}

static std::vector<GPU_Image *> canvas_list;

static int tsab_graphics_new_canvas(lua_State *L) {
	double w = luaL_checknumber(L, 1);
	double h = luaL_checknumber(L, 2);

	GPU_Image *image = GPU_CreateImage(w, h, GPU_FORMAT_RGBA);

	canvas_list.push_back(image);

	GPU_SetImageFilter(image, GPU_FILTER_NEAREST);
	GPU_LoadTarget(image);

	lua_pushnumber(L, canvas_list.size() - 1);

	return 1;
}

static int tsab_graphics_new_image(lua_State *L) {
	const char *name = luaL_checkstring(L, 1);

	SDL_Surface *texture = IMG_Load(name);

	if (texture == nullptr) {
		lua_pushnumber(L, -1);
		std::cerr << IMG_GetError() << std::endl;
		return 1;
	}

	GPU_Image *image = GPU_CopyImageFromSurface(texture);
	canvas_list.push_back(image);

	GPU_SetImageFilter(image, GPU_FILTER_NEAREST);
	GPU_LoadTarget(image);

	lua_pushnumber(L, canvas_list.size() - 1);

	return 1;
}

static int tsab_graphics_set_canvas(lua_State *L) {
	int id = luaL_checknumber(L, 1);

	if (id > -1 && id < canvas_list.size()) {
		current_target = canvas_list[id];
	} else {
		current_target = nullptr;
	}

	return 0;
}

static SDL_Color current_color = { 255, 255, 255, 255 };
static float bg_color[] = { 0, 0, 0 };

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

static const char *check_string(lua_State *L, int index, const char *default_str) {
	if (lua_isstring(L, index)) {
		return lua_tostring(L, index);
	}

	return default_str;
}

static int tsab_graphics_clear(lua_State *L) {
	GPU_ClearRGB(current_target == nullptr ? screen : current_target->target, bg_color[0], bg_color[1], bg_color[2]);
	return 0;
}

static int active_shader = -1;

static int tsab_graphics_draw(lua_State *L) {
	if (active_shader > -1) {
		GPU_SetUniformf(GPU_GetUniformLocation(shaders[active_shader], "textured"), 1);
	}

	auto *target = current_target == nullptr ? screen : current_target->target;

	GPU_Image *what = nullptr;
	int id = luaL_checknumber(L, 1);

	if (id > -1 && id < canvas_list.size()) {
		what = canvas_list[id];
	}

	if (what == nullptr) {
		return 0;
	}

	double x = check_number(L, 2, 0);
	double y = check_number(L, 3, 0);
	double a = check_number(L, 4, 0);
	double ox = check_number(L, 5, 0);
	double oy = check_number(L, 6, 0);
	double sx = check_number(L, 7, 1);
	double sy = check_number(L, 8, 1);
	double src_x = check_number(L, 9, 0);
	double src_y = check_number(L, 10, 0);
	double src_w = check_number(L, 11, what->w);
	double src_h = check_number(L, 12, what->h);

	GPU_Rect r = GPU_MakeRect(src_x, src_y, src_w, src_h);
	GPU_BlitTransformX(what, &r, target, x, y, ox, oy, a, sx, sy);

	return 0;
}

static int tsab_graphics_circle(lua_State *L) {
	if (active_shader > -1) {
		GPU_SetUniformf(GPU_GetUniformLocation(shaders[active_shader], "textured"), 0);
	}

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

static int tsab_graphics_rectangle(lua_State *L) {
	if (active_shader > -1) {
		GPU_SetUniformf(GPU_GetUniformLocation(shaders[active_shader], "textured"), 0);
	}

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

static int tsab_graphics_ellipse(lua_State *L) {
	if (active_shader > -1) {
		GPU_SetUniformf(GPU_GetUniformLocation(shaders[active_shader], "textured"), 0);
	}

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

static int tsab_graphics_triangle(lua_State *L) {
	if (active_shader > -1) {
		GPU_SetUniformf(GPU_GetUniformLocation(shaders[active_shader], "textured"), 0);
	}

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
	if (active_shader > -1) {
		GPU_SetUniformf(GPU_GetUniformLocation(shaders[active_shader], "textured"), 0);
	}

	double x = luaL_checknumber(L, 1);
	double y = luaL_checknumber(L, 2);

	GPU_Pixel(current_target == nullptr ? screen : current_target->target, x + 0.5, y + 0.5, current_color);

	return 0;
}

static int tsab_graphics_line(lua_State *L) {
	if (active_shader > -1) {
		GPU_SetUniformf(GPU_GetUniformLocation(shaders[active_shader], "textured"), 0);
	}

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

static int tsab_graphics_set_clear_color(lua_State *L) {
	double r = check_number(L, 1, 1);
	double g = check_number(L, 2, 1);
	double b = check_number(L, 3, 1);

	bg_color[0] = clamp(r * 255, 0, 255);
	bg_color[1] = clamp(g * 255, 0, 255);
	bg_color[2] = clamp(b * 255, 0, 255);

	return 0;
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

static bool pushed;

static int tsab_graphics_camera(lua_State *L) {
	double x = check_number(L, 1, 0);
	double y = check_number(L, 2, 0);
	double s = check_number(L, 3, 1);

	GPU_MatrixMode(GPU_MODELVIEW);

	if (pushed) {
		GPU_PopMatrix();
	}

	if (lua_isnil(L, 1)) {
		pushed = false;
		return 0;
	}

	if (current_target == nullptr) {
		y *= -1;
	}

	pushed = true;
	GPU_PushMatrix();
	GPU_Scale(s, s, 1.0f);
	GPU_Translate(x / (current_target == nullptr ? screen->w / 2 : current_target->w / 2), y / (current_target == nullptr ? screen->h / 2 : current_target->h / 2), 0.0f);
	return 0;
}

static std::vector<TTF_Font *> fonts;
static TTF_Font *active_font;

static int tsab_graphics_new_font(lua_State *L) {
	const char *name = luaL_checkstring(L, 1);
	int size = check_number(L, 2, 14);

	TTF_Font *font = TTF_OpenFont(name, size);

	if (font == nullptr) {
		std::cerr << "Failed to load font " << name << ": " << TTF_GetError() << std::endl;
		return -1;
	}

	if (active_font == nullptr) {
		active_font = font;
	}

	fonts.push_back(font);
	lua_pushnumber(L, fonts.size() - 1);

	return 1;
}

static int tsab_graphics_set_font(lua_State *L) {
	int id = luaL_checknumber(L, 1);

	if (id > - 1 && id < fonts.size()) {
		active_font = fonts[id];
	}

	return 0;
}

static int tsab_graphics_print(lua_State *L) {
	if (active_shader > -1) {
		GPU_SetUniformf(GPU_GetUniformLocation(shaders[active_shader], "textured"), 1);
	}

	const char *text = luaL_checkstring(L, 1);
	double x = check_number(L, 2, 0);
	double y = check_number(L, 3, 0);
	double r = check_number(L, 4, 0);
	double sx = check_number(L, 5, 1);
	double sy = check_number(L, 6, 1);

	SDL_Surface *surface = TTF_RenderUTF8_Blended(active_font, text, current_color);
	GPU_Image *image = GPU_CopyImageFromSurface(surface);
	GPU_BlitTransformX(image, nullptr, current_target == nullptr ? screen : current_target->target, x + image->w/2, y + image->h/2, image->w/2, image->h/2, r, sx, sy);
	SDL_FreeSurface(surface);

	return 0;
}

static int tsab_graphics_get_size(lua_State *L) {
	int id = check_number(L, 1, -2);

	if (id == -2) {
		int w;
		int h;

		SDL_GetWindowSize(window, &w, &h);
		lua_pushnumber(L, w);
		lua_pushnumber(L, h);
	} else if (id == -1) {
		lua_pushnumber(L, screen->w);
		lua_pushnumber(L, screen->h);
	} else if (id > -1 && id < canvas_list.size()) {
		GPU_Image *image = canvas_list[id];

		lua_pushnumber(L, image->w);
		lua_pushnumber(L, image->h);
	} else {
		return 0;
	}

	return 2;
}

static int tsab_graphics_set_size(lua_State *L) {
	int w = luaL_checknumber(L, 1);
	int h = luaL_checknumber(L, 2);

	GPU_SetWindowResolution(w, h);
	return 0;
}

static int tsab_graphics_set_title(lua_State *L) {
	const char *title = luaL_checkstring(L, 1);

	SDL_SetWindowTitle(window, title);
	return 0;
}

/*
 * Shaders
 */

static std::vector<Uint32> shaders_separate;

static int tsab_shaders_new(lua_State *L) {
	Uint32 v = GPU_LoadShader(GPU_VERTEX_SHADER, "default.vert");

	if (!v) {
		GPU_LogError("Failed to load vertex shader: %s\n", GPU_GetShaderMessage());
	}

	shaders_separate.push_back(v);
	Uint32 f = GPU_LoadShader(GPU_FRAGMENT_SHADER, luaL_checkstring(L, 1));

	if (!f) {
		GPU_LogError("Failed to load fragment shader: %s\n", GPU_GetShaderMessage());
	}

	shaders_separate.push_back(f);
	Uint32 p = GPU_LinkShaders(v, f);

	if (!p) {
		GPU_LogError("Failed to link shader program: %s\n", GPU_GetShaderMessage());
	}

	shader_blocks.push_back(GPU_LoadShaderBlock(p, "gpu_Vertex", "gpu_TexCoord", "gpu_Color", "gpu_ModelViewProjectionMatrix"));
	shaders.push_back(p);
	lua_pushnumber(L, shaders.size() - 1);

	return 1;
}

static int tsab_shaders_set(lua_State *L) {
	if (lua_isnil(L, 1)) {
		active_shader = -1;
		GPU_ActivateShaderProgram(0, nullptr);
		return 0;
	}

	Uint32 p = luaL_checknumber(L, 1);

	if (p >= -1 && shaders.size() <= p) {
		return 0;
	}

	active_shader = p;

	GPU_ActivateShaderProgram(shaders[p], &shader_blocks[p]);

	return 0;
}

static int tsab_shaders_send_float(lua_State *L) {
	Uint32 p = luaL_checknumber(L, 1);
	const char *name = luaL_checkstring(L, 2);
	double value = luaL_checknumber(L, 3);
	GPU_SetUniformf(GPU_GetUniformLocation(shaders[p], name), value);

	return 0;
}

static int tsab_shaders_send_vec4(lua_State *L) {
	Uint32 p = luaL_checknumber(L, 1);
	const char *name = luaL_checkstring(L, 2);
	float r = luaL_checknumber(L, 3);
	float g = luaL_checknumber(L, 4);
	float b = luaL_checknumber(L, 5);
	float a = luaL_checknumber(L, 6);

	float values[] = { r, g, b, a };
	GPU_SetUniformfv(GPU_GetUniformLocation(shaders[p], name), 4, 1, (float *) values);

	return 0;
}

/*
 * Audio
 */

static std::vector<Mix_Music *> music_list;
static std::vector<Mix_Chunk *> sfx_list;

static int tsab_audio_new_music(lua_State *L) {
	const char *name = luaL_checkstring(L, 1);
	Mix_Music *music = Mix_LoadMUS(name);

	if (music == nullptr) {
		std::cerr << Mix_GetError() << std::endl;
		lua_pushnumber(L, -1);
		return 1;
	}

	music_list.push_back(music);
	lua_pushnumber(L, music_list.size() - 1);

	return 1;
}

static int tsab_audio_play_music(lua_State *L) {
	int id = luaL_checknumber(L, 1);
	bool looping = check_bool(L, 2, true);

	if (id > -1 && id < music_list.size()) {
		if (Mix_PlayMusic(music_list[id], looping ? -1 : 0) == -1) {
			std::cerr << Mix_GetError() << std::endl;
		}
	}

	return 0;
}

static int tsab_audio_new_sfx(lua_State *L) {
	const char *name = luaL_checkstring(L, 1);
	Mix_Chunk *sfx = Mix_LoadWAV(name);

	if (sfx == nullptr) {
		std::cerr << Mix_GetError() << std::endl;
		lua_pushnumber(L, -1);
		return 1;
	}

	sfx_list.push_back(sfx);
	lua_pushnumber(L, sfx_list.size() - 1);

	return 1;
}

static int tsab_audio_play_sfx(lua_State *L) {
	int id = luaL_checknumber(L, 1);

	if (id > -1 && id < sfx_list.size()) {
		if (Mix_PlayChannel(-1, sfx_list[id], 0) == -1) {
			std::cerr << Mix_GetError() << std::endl;
		}
	}

	return 0;
}

static int tsab_audio_set_general_volume(lua_State *L) {
	double value = luaL_checknumber(L, 1);
	std::cout << "set volume to " << value << "\n";
	Mix_Volume(-1, std::min<int>(std::max<int>(0, value * 128), 128));

	return 0;
}

static int tsab_audio_set_sfx_volume(lua_State *L) {
	double id = luaL_checknumber(L, 1);
	double value = luaL_checknumber(L, 2);

	if (id > -1 && id < sfx_list.size()) {
		Mix_VolumeChunk(sfx_list[id], std::min<int>(std::max<int>(0, value * 128), 128));
		std::cerr << std::min<int>(std::max<int>(0, value * 128), 128) << std::endl;
	}

	return 0;
}

static int tsab_audio_set_music_volume(lua_State *L) {
	double value = luaL_checknumber(L, 1);
	Mix_VolumeMusic(std::min<int>(std::max<int>(0, value * 128), 128));

	return 0;
}

SDL_Renderer *renderer;
lua_State *L;

int tsab_init(int arg, char **argv) {
	if (arg >= 2) {
		working_dir = std::string(argv[1]);

		if (working_dir[working_dir.length() - 1] != '/') {
			working_dir += "/";
		}

		chdir(working_dir.c_str());
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
	
	// return 0;

	GPU_SetDebugLevel(GPU_DEBUG_LEVEL_MAX);
	GPU_SetRequiredFeatures(GPU_FEATURE_BASIC_SHADERS);
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
	SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");

	// Register API
	// Main API
	lua_register(L, "tsab_get_time", tsab_get_time);
	lua_register(L, "tsab_get_fps", tsab_get_fps);
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
	lua_register(L, "tsab_graphics_set_clear_color", tsab_graphics_set_clear_color);
	lua_register(L, "tsab_graphics_clear", tsab_graphics_clear);
	lua_register(L, "tsab_graphics_get_color", tsab_graphics_get_color);
	lua_register(L, "tsab_graphics_set_size", tsab_graphics_set_size);
	lua_register(L, "tsab_graphics_set_title", tsab_graphics_set_title);
	lua_register(L, "tsab_graphics_get_size", tsab_graphics_get_size);
	lua_register(L, "tsab_graphics_draw", tsab_graphics_draw);
	lua_register(L, "tsab_graphics_circle", tsab_graphics_circle);
	lua_register(L, "tsab_graphics_rectangle", tsab_graphics_rectangle);
	lua_register(L, "tsab_graphics_point", tsab_graphics_point);
	lua_register(L, "tsab_graphics_line", tsab_graphics_line);
	lua_register(L, "tsab_graphics_ellipse", tsab_graphics_ellipse);
	lua_register(L, "tsab_graphics_triangle", tsab_graphics_triangle);
	lua_register(L, "tsab_graphics_camera", tsab_graphics_camera);
	lua_register(L, "tsab_graphics_new_font", tsab_graphics_new_font);
	lua_register(L, "tsab_graphics_set_font", tsab_graphics_set_font);
	lua_register(L, "tsab_graphics_print", tsab_graphics_print);
	lua_register(L, "tsab_graphics_new_image", tsab_graphics_new_image);
	// Shaders API
	lua_register(L, "tsab_shaders_new", tsab_shaders_new);
	lua_register(L, "tsab_shaders_set", tsab_shaders_set);
	lua_register(L, "tsab_shaders_send_float", tsab_shaders_send_float);
	lua_register(L, "tsab_shaders_send_vec4", tsab_shaders_send_vec4);
	// Audio API
	lua_register(L, "tsab_audio_new_music", tsab_audio_new_music);
	lua_register(L, "tsab_audio_play_music", tsab_audio_play_music);
	lua_register(L, "tsab_audio_new_sfx", tsab_audio_new_sfx);
	lua_register(L, "tsab_audio_play_sfx", tsab_audio_play_sfx);
	lua_register(L, "tsab_audio_set_sfx_volume", tsab_audio_set_sfx_volume);
	lua_register(L, "tsab_audio_set_music_volume", tsab_audio_set_music_volume);
	lua_register(L, "tsab_audio_set_general_volume", tsab_audio_set_general_volume);

	// Create window
	int flags = 0;

	if (resizeable) {
		flags |= SDL_WINDOW_RESIZABLE;
	}

	window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, flags);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	GPU_SetInitWindow(SDL_GetWindowID(window));
	SDL_SetWindowMinimumSize(window, window_min_width, window_min_height);
	screen = GPU_Init(window_width, window_height, GPU_DEFAULT_INIT_FLAGS);
	GPU_ClearRGB(screen, bg_color[0], bg_color[1], bg_color[2]);

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

	// Init key map
	setup_key_map();
	// Call init
	lua_pushcfunction(L, traceback);
	lua_getglobal(L, "tsab_init");

	if (lua_pcall(L, 0, 0, lua_gettop(L) - 1) != 0) {
		report_lua_error(L);
	}

	// Event / input vars
	input_previous_keyboard_state = new Uint8[512];
	input_current_keyboard_state = SDL_GetKeyboardState(nullptr);

	input_previous_mouse_state = new Uint8[12];
	input_current_mouse_state = new Uint8[12];

	input_previous_gamepad_button_state = new Uint8[SDL_CONTROLLER_BUTTON_MAX - SDL_CONTROLLER_BUTTON_INVALID + 1];
	input_current_gamepad_button_state = new Uint8[SDL_CONTROLLER_BUTTON_MAX - SDL_CONTROLLER_BUTTON_INVALID + 1];
	input_gamepad_axis_state = new Sint16[SDL_CONTROLLER_AXIS_MAX - SDL_CONTROLLER_AXIS_INVALID + 1];

	return 0;
}

Uint64 timer_now = SDL_GetPerformanceCounter();
Uint64 timer_last = 0;
double timer_dt = 0;
double timer_fixed_dt = 1.0 / 60.0;
double timer_accumulator = 0;
int frame;
Uint32 start;
int w;
int h;
SDL_Event event;

void tsab_loop() {
	SDL_ShowWindow(window);

	while (running) {
		tsab_loop_step();
	}
}

void tsab_loop_step() {
	start = SDL_GetTicks();

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
			case SDL_QUIT:
				running = false;
				break;
			case SDL_WINDOWEVENT:
				switch (event.window.event) {
					case SDL_WINDOWEVENT_RESIZED:
						lua_pushcfunction(L, traceback);
						lua_getglobal(L, "tsab_resize");

						SDL_GetWindowSize(window, &w, &h);
						GPU_SetWindowResolution(w, h);

						lua_pushnumber(L, w);
						lua_pushnumber(L, h);

						if (lua_pcall(L, 2, 0, lua_gettop(L) - 3) != 0) {
							report_lua_error(L);
						}

						break;
				}
				break;
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
			default:
				break;
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

	GPU_ClearRGB(screen, bg_color[0], bg_color[1], bg_color[2]);

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

	if (frame % 40 == 0) {
		fps = 1000.0 / ((double) (SDL_GetTicks() - start));
	}

	frame ++;
}

void tsab_quit() {
	for (int i = 0; i < canvas_list.size(); i++) {
		GPU_FreeImage(canvas_list[i]);
	}

	for (int i = 0; i < shaders_separate.size(); i++) {
		GPU_FreeShader(shaders_separate[i]);
	}

	for (int i = 0; i < shaders.size(); i++) {
		GPU_FreeShaderProgram(shaders[i]);
	}

	for (int i = 0; i < music_list.size(); i++) {
		Mix_FreeMusic(music_list[i]);
	}

	delete input_previous_mouse_state;
	delete input_current_mouse_state;
	delete input_previous_keyboard_state;

	Mix_CloseAudio();
	TTF_Quit();
	GPU_Quit();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	// Call destroy
	lua_pushcfunction(L, traceback);
	lua_getglobal(L, "tsab_destroy");

	if (lua_pcall(L, 0, 0, lua_gettop(L) - 1) != 0) {
		report_lua_error(L);
	}
}