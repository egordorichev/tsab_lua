#ifndef TSAB_SHADERS
#define TSAB_SHADERS

#include <SDL2/SDL.h>

#include <vector>

#include "lua.hpp"

void tsab_shaders_register_api(lua_State *L);
void tsab_shaders_quit();
int tsab_shaders_get_active();
Uint32 tsab_shaders_get_active_shader();

int tsab_shaders_new(lua_State *L);
int tsab_shaders_set(lua_State *L);
int tsab_shaders_send_float(lua_State *L);
int tsab_shaders_send_int(lua_State *L);
int tsab_shaders_send_vec4(lua_State *L);
int tsab_shaders_send_vec3(lua_State *L);
int tsab_shaders_send_vec2(lua_State *L);

#endif