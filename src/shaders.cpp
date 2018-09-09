#include <SDL_GPU/SDL_gpu.h>

#include <iostream>

#include "shaders.hpp"

std::vector<Uint32> shaders_separate;
std::vector<GPU_ShaderBlock> shader_blocks;
int active_shader = -1;
std::vector<Uint32> shaders;

static const char *default_vert =
"attribute vec3 gpu_Vertex;\n"
"attribute vec2 gpu_TexCoord;\n"
"attribute vec4 gpu_Color;\n"
"uniform mat4 gpu_ModelViewProjectionMatrix;\n"
"\n"
"varying vec4 color;\n"
"varying vec2 texCoord;\n"
"\n"
"void main() {\n"
"\tcolor = gpu_Color;\n"
"\ttexCoord = vec2(gpu_TexCoord);\n"
"\tgl_Position = gpu_ModelViewProjectionMatrix * vec4(gpu_Vertex, 1.0);\n"
"}";

void tsab_shaders_register_api(lua_State *L) {
	lua_register(L, "tsab_shaders_new", tsab_shaders_new);
	lua_register(L, "tsab_shaders_set", tsab_shaders_set);
	lua_register(L, "tsab_shaders_send_float", tsab_shaders_send_float);
	lua_register(L, "tsab_shaders_send_int", tsab_shaders_send_int);
	lua_register(L, "tsab_shaders_send_vec4", tsab_shaders_send_vec4);
	lua_register(L, "tsab_shaders_send_vec3", tsab_shaders_send_vec3);
	lua_register(L, "tsab_shaders_send_vec2", tsab_shaders_send_vec2);
}

void tsab_shaders_quit() {
	for (int i = 0; i < shaders_separate.size(); i++) {
		GPU_FreeShader(shaders_separate[i]);
	}

	for (int i = 0; i < shaders.size(); i++) {
		GPU_FreeShaderProgram(shaders[i]);
	}
}

int tsab_shaders_get_active() {
	return active_shader;
}

Uint32 tsab_shaders_get_active_shader() {
	return shaders[active_shader];
}

int tsab_shaders_new(lua_State *L) {
	Uint32 v = GPU_CompileShader(GPU_VERTEX_SHADER, default_vert);

	if (!v) {
		GPU_LogError("Failed to load vertex shader: %s\n", GPU_GetShaderMessage());
	}

	shaders_separate.push_back(v);
	Uint32 f;
	const char *name = luaL_checkstring(L, 1);

	if (check_bool(L, 2, false)) {
		f = GPU_CompileShader(GPU_FRAGMENT_SHADER, name);
	} else {
		f = GPU_LoadShader(GPU_FRAGMENT_SHADER, name);
	}

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

int tsab_shaders_set(lua_State *L) {
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

int tsab_shaders_send_float(lua_State *L) {
	Uint32 p = luaL_checknumber(L, 1);
	const char *name = luaL_checkstring(L, 2);
	double value = luaL_checknumber(L, 3);
	GPU_SetUniformf(GPU_GetUniformLocation(shaders[p], name), value);

	return 0;
}

int tsab_shaders_send_int(lua_State *L) {
	Uint32 p = luaL_checknumber(L, 1);
	const char *name = luaL_checkstring(L, 2);
	double value = luaL_checknumber(L, 3);
	GPU_SetUniformi(GPU_GetUniformLocation(shaders[p], name), value);

	return 0;
}

int tsab_shaders_send_vec4(lua_State *L) {
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

int tsab_shaders_send_vec2(lua_State *L) {
	Uint32 p = luaL_checknumber(L, 1);
	const char *name = luaL_checkstring(L, 2);
	float r = luaL_checknumber(L, 3);
	float g = luaL_checknumber(L, 4);

	float values[] = { r, g };
	GPU_SetUniformfv(GPU_GetUniformLocation(shaders[p], name), 2, 1, (float *) values);

	return 0;
}

int tsab_shaders_send_vec3(lua_State *L) {
	Uint32 p = luaL_checknumber(L, 1);
	const char *name = luaL_checkstring(L, 2);
	float r = luaL_checknumber(L, 3);
	float g = luaL_checknumber(L, 4);
	float b = luaL_checknumber(L, 5);

	float values[] = { r, g, b };
	GPU_SetUniformfv(GPU_GetUniformLocation(shaders[p], name), 3, 1, (float *) values);

	return 0;
}