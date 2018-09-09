#ifndef TSAB_GRAPHICS
#define TSAB_GRAPHICS

#include <SDL2/SDL.h>
#include <SDL_GPU/SDL_gpu.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include "lua.hpp"

void tsab_graphics_init(int w, int h, int mw, int mh, char *title, int flags);
void tsab_graphics_get_ready();
void tsab_graphics_quit();
void tsab_graphics_register_api(lua_State *L);
void tsab_graphics_resize(int *w, int *h);
void tsab_graphics_clear_screen();
GPU_Target *tsab_graphics_get_current_target();

int tsab_graphics_flip(lua_State *L);
int tsab_graphics_get_default_canvas(lua_State *L);
int tsab_graphics_new_canvas(lua_State *L);
int tsab_graphics_new_image(lua_State *L);
int tsab_graphics_set_canvas(lua_State *L);
int tsab_graphics_clear(lua_State *L);
int tsab_graphics_draw(lua_State *L);
int tsab_graphics_circle(lua_State *L);
int tsab_graphics_rectangle(lua_State *L);
int tsab_graphics_ellipse(lua_State *L);
int tsab_graphics_triangle(lua_State *L);
int tsab_graphics_point(lua_State *L);
int tsab_graphics_line(lua_State *L);
int tsab_graphics_set_clear_color(lua_State *L);
int tsab_graphics_set_color(lua_State *L);
int tsab_graphics_get_color(lua_State *L);
int tsab_graphics_camera(lua_State *L);
int tsab_graphics_new_font(lua_State *L);
int tsab_graphics_set_font(lua_State *L);
int tsab_graphics_print(lua_State *L);
int tsab_graphics_get_size(lua_State *L);
int tsab_graphics_set_size(lua_State *L);
int tsab_graphics_set_title(lua_State *L);

#endif