#ifndef TSAB_UI
#define TSAB_UI

#include <SDL2/SDL.h>

#include "lua.hpp"

void tsab_ui_init();
void tsab_ui_register_api(lua_State *L);
void tsab_ui_quit();
void tsab_ui_handle_event(SDL_Event *event);

int tsab_ui_frame(lua_State *L);
int tsab_ui_render(lua_State *L);
int tsab_ui_begin(lua_State *L);
int tsab_ui_end(lua_State *L);
int tsab_ui_button(lua_State *L);
int tsab_ui_label(lua_State *L);
int tsab_ui_text_input(lua_State *L);
int tsab_ui_collapsing_header(lua_State *L);
int tsab_ui_tree_node(lua_State *L);
int tsab_ui_pop_node(lua_State *L);
int tsab_ui_same_line(lua_State *L);
int tsab_ui_slider_float2(lua_State *L);
int tsab_ui_input_float2(lua_State *L);

#endif