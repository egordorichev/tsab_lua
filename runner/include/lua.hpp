#ifndef LUA_HPP
#define LUA_HPP

extern "C" {
#include <Lua/lua.h>
#include <Lua/lauxlib.h>
#include <Lua/lualib.h>
}

bool check_bool(lua_State *L, int index, bool default_bool);
double check_number(lua_State *L, int index, double default_number);
const char *check_string(lua_State *L, int index, const char *default_str);

#endif