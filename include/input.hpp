#ifndef TSAB_INPUT_HPP
#define TSAB_INPUT_HPP

#include <SDL2/SDL.h>

#include "lua.hpp"

#define MOUSE_1 0
#define MOUSE_2 1
#define MOUSE_3 2
#define MOUSE_4 3
#define MOUSE_5 4
#define MOUSE_WHEEL_UP 5
#define MOUSE_WHEEL_DOWN 6
#define MOUSE_WHEEL_LEFT 7
#define MOUSE_WHEEL_RIGHT 8

void tsab_input_init();
void tsab_input_register_api(lua_State *L);
void tsab_input_quit();
void tsab_input_update();
void tsab_input_handle_event(SDL_Event *event);

int tsab_input_get_axis(lua_State *L);
int tsab_input_get_mouse_position(lua_State *L);
int tsab_input_was_released(lua_State *L);
int tsab_input_was_pressed(lua_State *L);
int tsab_input_is_down(lua_State *L);

#endif