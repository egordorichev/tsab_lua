#include "lua.hpp"

bool check_bool(lua_State *L, int index, bool default_bool) {
	if (lua_isboolean( L, index)) {
		return static_cast<bool>(lua_toboolean(L, index));
	}

	return default_bool;
}

double check_number(lua_State *L, int index, double default_number) {
	if (lua_isnumber(L, index)) {
		return lua_tonumber(L, index);
	}

	return default_number;
}

const char *check_string(lua_State *L, int index, const char *default_str) {
	if (lua_isstring(L, index)) {
		return lua_tostring(L, index);
	}

	return default_str;
}