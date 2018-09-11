#ifndef TSAB_AUDIO_HPP
#define TSAB_AUDIO_HPP

#include "lua.hpp"

void tsab_audio_init();
void tsab_audio_register_api(lua_State *L);
void tsab_audio_quit();

int tsab_audio_new_music(lua_State *L);
int tsab_audio_play_music(lua_State *L);
int tsab_audio_new_sfx(lua_State *L);
int tsab_audio_play_sfx(lua_State *L);
int tsab_audio_set_general_volume(lua_State *L);
int tsab_audio_set_sfx_volume(lua_State *L);
int tsab_audio_set_music_volume(lua_State *L);

#endif