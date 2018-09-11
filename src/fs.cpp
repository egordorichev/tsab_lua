#include <string>
#include <iostream>
#include <experimental/filesystem>

#include "fs.hpp"

#ifdef _WIN32
#include <direct.h>
#define cwd _getcwd
#define cd _chdir
#else
#include "unistd.h"
#define cwd getcwd
#define cd chdir
#endif

namespace fs = std::experimental::filesystem;
static std::string working_dir = "./";

void tsab_fs_register_api(lua_State *L) {
	lua_register(L, "tsab_fs_get_directory_files", tsab_fs_get_directory_files);
	lua_register(L, "tsab_fs_is_directory", tsab_fs_is_directory);
	lua_register(L, "tsab_fs_get_last_modified", tsab_fs_get_last_modified);
}

void tsab_fs_set_working_dir(const char *dir) {
	working_dir = dir;

	if (working_dir[working_dir.length() - 1] != '/') {
		working_dir += "/";
	}

	chdir(working_dir.c_str());
}

int tsab_fs_get_directory_files(lua_State *L) {
	std::string path = check_string(L, 1, ".");

	if (path.empty()) {
		path = ".";
	}

	lua_newtable(L);

	if (!fs::exists(path)) {
		return 1;
	}

	int i = 0;

	auto dir = fs::directory_iterator(path);

	for (auto & p : dir) {
		lua_pushstring(L, p.path().generic_string().c_str());
		lua_rawseti(L, -2, i);

		i++;
	}

	return 1;
}

int tsab_fs_is_directory(lua_State *L) {
	std::error_code ec;
	bool is = fs::is_directory(luaL_checkstring(L, 1), ec);

	if (ec) {
	}

	lua_pushboolean(L, is);

	return 1;
}

int tsab_fs_get_last_modified(lua_State *L) {
	auto time = fs::last_write_time(luaL_checkstring(L, 1));
	lua_pushnumber(L, decltype(time)::clock::to_time_t(time));
	return 1;
}