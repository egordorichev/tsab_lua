#include <SDL_mixer.h>

#include <vector>
#include <iostream>

#include "audio.hpp"

static std::vector<Mix_Music *> music_list;
static std::vector<Mix_Chunk *> sfx_list;

void tsab_audio_init() {
	Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
}

void tsab_audio_register_api(lua_State *L) {
	lua_register(L, "tsab_audio_new_music", tsab_audio_new_music);
	lua_register(L, "tsab_audio_play_music", tsab_audio_play_music);
	lua_register(L, "tsab_audio_new_sfx", tsab_audio_new_sfx);
	lua_register(L, "tsab_audio_play_sfx", tsab_audio_play_sfx);
	lua_register(L, "tsab_audio_set_sfx_volume", tsab_audio_set_sfx_volume);
	lua_register(L, "tsab_audio_set_music_volume", tsab_audio_set_music_volume);
	lua_register(L, "tsab_audio_set_general_volume", tsab_audio_set_general_volume);
}

void tsab_audio_quit() {
	for (int i = 0; i < music_list.size(); i++) {
		Mix_FreeMusic(music_list[i]);
	}

	Mix_CloseAudio();
}

int tsab_audio_new_music(lua_State *L) {
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

int tsab_audio_play_music(lua_State *L) {
	int id = luaL_checknumber(L, 1);
	bool looping = check_bool(L, 2, true);

	if (id > -1 && id < music_list.size()) {
		if (Mix_PlayMusic(music_list[id], looping ? -1 : 0) == -1) {
			std::cerr << Mix_GetError() << std::endl;
		}
	}

	return 0;
}

int tsab_audio_new_sfx(lua_State *L) {
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

int tsab_audio_play_sfx(lua_State *L) {
	int id = luaL_checknumber(L, 1);

	if (id > -1 && id < sfx_list.size()) {
		if (Mix_PlayChannel(-1, sfx_list[id], 0) == -1) {
			std::cerr << Mix_GetError() << std::endl;
		}
	}

	return 0;
}

int tsab_audio_set_general_volume(lua_State *L) {
	double value = luaL_checknumber(L, 1);
	std::cout << "set volume to " << value << "\n";
	Mix_Volume(-1, std::min<int>(std::max<int>(0, value * 128), 128));

	return 0;
}

int tsab_audio_set_sfx_volume(lua_State *L) {
	double id = luaL_checknumber(L, 1);
	double value = luaL_checknumber(L, 2);

	if (id > -1 && id < sfx_list.size()) {
		Mix_VolumeChunk(sfx_list[id], std::min<int>(std::max<int>(0, value * 128), 128));
		std::cerr << std::min<int>(std::max<int>(0, value * 128), 128) << std::endl;
	}

	return 0;
}

int tsab_audio_set_music_volume(lua_State *L) {
	double value = luaL_checknumber(L, 1);
	Mix_VolumeMusic(std::min<int>(std::max<int>(0, value * 128), 128));

	return 0;
}