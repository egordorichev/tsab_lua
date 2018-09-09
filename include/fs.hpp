#ifndef TSAB_FS
#define TSAB_FS

#include "lua.hpp"

void tsab_fs_register_api(lua_State *L);
void tsab_fs_set_working_dir(const char *dir);

int tsab_fs_get_directory_files(lua_State *L);
int tsab_fs_is_directory(lua_State *L);
int tsab_fs_get_last_modified(lua_State *L);

#endif