#include <SDL2/SDL.h>
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_sdl.h>
#include <ImGui/imgui_impl_opengl3.h>

#include "ui.hpp"

#include <map>
#include <cstring>

void tsab_ui_init() {
#if __APPLE__
	// GL 3.2 Core + GLSL 150
  const char* glsl_version = "#version 150";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForOpenGL(SDL_GL_GetCurrentWindow(), SDL_GL_GetCurrentContext());
	ImGui_ImplOpenGL3_Init(glsl_version);
	ImGui::StyleColorsClassic();
}

void tsab_ui_register_api(lua_State *L) {
	lua_register(L, "tsab_ui_frame", tsab_ui_frame);
	lua_register(L, "tsab_ui_button", tsab_ui_button);
	lua_register(L, "tsab_ui_text_input", tsab_ui_text_input);
	lua_register(L, "tsab_ui_label", tsab_ui_label);
	lua_register(L, "tsab_ui_render", tsab_ui_render);
	lua_register(L, "tsab_ui_begin", tsab_ui_begin);
	lua_register(L, "tsab_ui_end", tsab_ui_end);
	lua_register(L, "tsab_ui_collapsing_header", tsab_ui_collapsing_header);
	lua_register(L, "tsab_ui_tree_node", tsab_ui_tree_node);
	lua_register(L, "tsab_ui_pop_node", tsab_ui_pop_node);
	lua_register(L, "tsab_ui_same_line", tsab_ui_same_line);
	lua_register(L, "tsab_ui_slider_float2", tsab_ui_slider_float2);
	lua_register(L, "tsab_ui_input_float2", tsab_ui_input_float2);
}

void tsab_ui_handle_event(SDL_Event *event) {
	ImGui_ImplSDL2_ProcessEvent(event);
}

// Compares two strings in a map
struct compare
{
	bool operator() (char *a, char *b) {
		return std::strcmp(a, b) < 0;
	}
};

void tsab_ui_quit() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

int tsab_ui_frame(lua_State *L) {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(SDL_GL_GetCurrentWindow());
	ImGui::NewFrame();
	return 0;
}

int tsab_ui_button(lua_State *L) {
	lua_pushboolean(L, ImGui::Button(check_string(L, 1, "Button")));
	return 1;
}

int tsab_ui_label(lua_State *L) {
	const char *str = luaL_checkstring(L, 1);

	if (lua_isstring(L, 2)) {
		ImGui::LabelText(luaL_checkstring(L, 2), str);
	} else {
		ImGui::Text("%s", str);
	}

	return 0;
}

int tsab_ui_text_input(lua_State *L) {
	char *buffer = (char *) luaL_checkstring(L, 2);
	bool v = ImGui::InputText(luaL_checkstring(L, 1), buffer, 64);

	lua_pushstring(L, buffer);
	lua_pushboolean(L, v);

	return 2;
}

int tsab_ui_render(lua_State *L) {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	return 0;
}

int tsab_ui_collapsing_header(lua_State *L) {
	lua_pushboolean(L, ImGui::CollapsingHeader(luaL_checkstring(L, 1)));
	return 1;
}

int tsab_ui_tree_node(lua_State *L) {
	lua_pushboolean(L, ImGui::TreeNode(luaL_checkstring(L, 1)));
	return 1;
}

int tsab_ui_pop_node(lua_State *L) {
	ImGui::TreePop();
	return 0;
}

int tsab_ui_begin(lua_State *L) {
	ImGui::Begin(luaL_checkstring(L, 1));
	return 0;
}

int tsab_ui_end(lua_State *L) {
	ImGui::End();
	return 0;
}

int tsab_ui_same_line(lua_State *L) {
	ImGui::SameLine(0, 5);
	return 0;
}

int tsab_ui_slider_float2(lua_State *L) {
	float values[2] = {
		static_cast<float>(luaL_checknumber(L, 2)),
		static_cast<float>(luaL_checknumber(L, 3))
	};

	bool v = ImGui::SliderFloat2(luaL_checkstring(L, 1), values, check_number(L, 3, 0), check_number(L, 4, 10), "%.2f", check_number(L, 5, 1));

	lua_pushnumber(L, values[0]);
	lua_pushnumber(L, values[1]);
	lua_pushboolean(L, v);

	return 3;
}

int tsab_ui_input_float2(lua_State *L) {
	float values[2] = {
		static_cast<float>(luaL_checknumber(L, 2)),
		static_cast<float>(luaL_checknumber(L, 3))
	};

	bool v = ImGui::InputFloat2(luaL_checkstring(L, 1), values);

	lua_pushnumber(L, values[0]);
	lua_pushnumber(L, values[1]);
	lua_pushboolean(L, v);

	return 3;
}