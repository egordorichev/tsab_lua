#include <iostream>

#include "graphics.hpp"
#include "shaders.hpp"

SDL_Color current_color = { 255, 255, 255, 255 };
float bg_color[] = { 0, 0, 0 };
GPU_Target *screen;
SDL_Window *window;
SDL_Renderer *renderer;
GPU_Image *current_target;
std::vector<GPU_Image *> canvas_list;
std::vector<TTF_Font *> fonts;
TTF_Font *active_font;

int clamp(int val, int min, int max) {
	return std::min(std::max(min, val), max);
}

void tsab_graphics_init(int w, int h, int mw, int mh, char *title, int flags) {
	GPU_SetDebugLevel(GPU_DEBUG_LEVEL_MAX);
	GPU_SetRequiredFeatures(GPU_FEATURE_BASIC_SHADERS);

	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();

	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, flags);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	GPU_SetInitWindow(SDL_GetWindowID(window));
	SDL_SetWindowMinimumSize(window, mw, mh);

	screen = GPU_Init(w, h, GPU_DEFAULT_INIT_FLAGS);
	GPU_ClearRGB(screen, bg_color[0], bg_color[1], bg_color[2]);
}

void tsab_graphics_get_ready() {
	SDL_ShowWindow(window);
}

void tsab_graphics_quit() {
	for (int i = 0; i < canvas_list.size(); i++) {
		GPU_FreeImage(canvas_list[i]);
	}

	TTF_Quit();
	GPU_Quit();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

void tsab_graphics_clear_screen() {
	GPU_ClearRGB(screen, bg_color[0], bg_color[1], bg_color[2]);
}

void tsab_graphics_register_api(lua_State *L) {
	lua_register(L, "tsab_graphics_get_default_canvas", tsab_graphics_get_default_canvas);
	lua_register(L, "tsab_graphics_new_canvas", tsab_graphics_new_canvas);
	lua_register(L, "tsab_graphics_set_canvas", tsab_graphics_set_canvas);
	lua_register(L, "tsab_graphics_set_color", tsab_graphics_set_color);
	lua_register(L, "tsab_graphics_set_clear_color", tsab_graphics_set_clear_color);
	lua_register(L, "tsab_graphics_clear", tsab_graphics_clear);
	lua_register(L, "tsab_graphics_get_color", tsab_graphics_get_color);
	lua_register(L, "tsab_graphics_set_size", tsab_graphics_set_size);
	lua_register(L, "tsab_graphics_set_title", tsab_graphics_set_title);
	lua_register(L, "tsab_graphics_get_size", tsab_graphics_get_size);
	lua_register(L, "tsab_graphics_draw", tsab_graphics_draw);
	lua_register(L, "tsab_graphics_circle", tsab_graphics_circle);
	lua_register(L, "tsab_graphics_rectangle", tsab_graphics_rectangle);
	lua_register(L, "tsab_graphics_point", tsab_graphics_point);
	lua_register(L, "tsab_graphics_line", tsab_graphics_line);
	lua_register(L, "tsab_graphics_ellipse", tsab_graphics_ellipse);
	lua_register(L, "tsab_graphics_triangle", tsab_graphics_triangle);
	lua_register(L, "tsab_graphics_camera", tsab_graphics_camera);
	lua_register(L, "tsab_graphics_new_font", tsab_graphics_new_font);
	lua_register(L, "tsab_graphics_set_font", tsab_graphics_set_font);
	lua_register(L, "tsab_graphics_print", tsab_graphics_print);
	lua_register(L, "tsab_graphics_new_image", tsab_graphics_new_image);
	lua_register(L, "tsab_graphics_flip", tsab_graphics_flip);
}

void tsab_graphics_resize(int *w, int *h) {
	SDL_GetWindowSize(window, w, h);
	GPU_SetWindowResolution(*w, *h);
}

int tsab_graphics_flip(lua_State *L) {
	GPU_Flip(screen);
}

int tsab_graphics_get_default_canvas(lua_State *L) {
	lua_pushnumber(L, -1);
	lua_pushnumber(L, screen->w);
	lua_pushnumber(L, screen->h);

	return 3;
}

int tsab_graphics_new_canvas(lua_State *L) {
	double w = luaL_checknumber(L, 1);
	double h = luaL_checknumber(L, 2);

	GPU_Image *image = GPU_CreateImage(w, h, GPU_FORMAT_RGBA);

	canvas_list.push_back(image);

	GPU_SetImageFilter(image, GPU_FILTER_NEAREST);
	GPU_LoadTarget(image);

	lua_pushnumber(L, canvas_list.size() - 1);

	return 1;
}

int tsab_graphics_new_image(lua_State *L) {
	const char *name = luaL_checkstring(L, 1);

	SDL_Surface *texture = IMG_Load(name);

	if (texture == nullptr) {
		lua_pushnumber(L, -1);
		std::cerr << IMG_GetError() << std::endl;
		return 1;
	}

	GPU_Image *image = GPU_CopyImageFromSurface(texture);
	canvas_list.push_back(image);

	GPU_SetImageFilter(image, GPU_FILTER_NEAREST);
	GPU_LoadTarget(image);

	lua_pushnumber(L, canvas_list.size() - 1);

	return 1;
}

int tsab_graphics_set_canvas(lua_State *L) {
	int id = luaL_checknumber(L, 1);

	if (id > -1 && id < canvas_list.size()) {
		current_target = canvas_list[id];
	} else {
		current_target = nullptr;
	}

	return 0;
}

int tsab_graphics_clear(lua_State *L) {
	if (lua_isnumber(L, 1)) {
		float r = clamp(luaL_checknumber(L, 1) * 255, 0, 255);
		float g = clamp(luaL_checknumber(L, 2) * 255, 0, 255);
		float b = clamp(luaL_checknumber(L, 3) * 255, 0, 255);
		float a = clamp(check_number(L, 4, 1) * 255, 0, 255);

		GPU_ClearRGBA(current_target == nullptr ? screen : current_target->target, r, g, b, a);
	} else {
		GPU_ClearRGB(current_target == nullptr ? screen : current_target->target, bg_color[0], bg_color[1], bg_color[2]);
	}

	return 0;
}

int tsab_graphics_draw(lua_State *L) {
	if (tsab_shaders_get_active() > -1) {
		GPU_SetUniformf(GPU_GetUniformLocation(tsab_shaders_get_active_shader(), "textured"), 1);
	}

	auto *target = current_target == nullptr ? screen : current_target->target;

	GPU_Image *what = nullptr;
	int id = luaL_checknumber(L, 1);

	if (id > -1 && id < canvas_list.size()) {
		what = canvas_list[id];
	}

	if (what == nullptr) {
		return 0;
	}

	double x = check_number(L, 2, 0);
	double y = check_number(L, 3, 0);
	double a = check_number(L, 4, 0);
	double ox = check_number(L, 5, 0);
	double oy = check_number(L, 6, 0);
	double sx = check_number(L, 7, 1);
	double sy = check_number(L, 8, 1);
	double src_x = check_number(L, 9, 0);
	double src_y = check_number(L, 10, 0);
	double src_w = check_number(L, 11, what->w);
	double src_h = check_number(L, 12, what->h);

	GPU_Rect r = GPU_MakeRect(src_x, src_y, src_w, src_h);
	GPU_BlitTransformX(what, &r, target, x, y, ox, oy, a, sx, sy);

	return 0;
}

GPU_Target *tsab_graphics_get_current_target() {
	return current_target == nullptr ? screen : current_target->target;
}

int tsab_graphics_circle(lua_State *L) {
	if (tsab_shaders_get_active() > -1) {
		GPU_SetUniformf(GPU_GetUniformLocation(tsab_shaders_get_active_shader(), "textured"), 0);
	}

	double x = luaL_checknumber(L, 1);
	double y = luaL_checknumber(L, 2);
	double r = luaL_checknumber(L, 3);
	bool filled = check_bool(L, 4, true);

	if (filled) {
		GPU_CircleFilled(current_target == nullptr ? screen : current_target->target, x, y, r, current_color);
	} else {
		GPU_Circle(current_target == nullptr ? screen : current_target->target, x, y, r, current_color);
	}

	return 0;
}

int tsab_graphics_rectangle(lua_State *L) {
	if (tsab_shaders_get_active() > -1) {
		GPU_SetUniformf(GPU_GetUniformLocation(tsab_shaders_get_active_shader(), "textured"), 0);
	}

	double x = luaL_checknumber(L, 1);
	double y = luaL_checknumber(L, 2);
	double w = luaL_checknumber(L, 3);
	double h = luaL_checknumber(L, 4);
	bool filled = check_bool(L, 5, true);

	if (filled) {
		GPU_RectangleFilled(current_target == nullptr ? screen : current_target->target, x, y, x + w, y + h, current_color);
	} else {
		GPU_Rectangle(current_target == nullptr ? screen : current_target->target, x, y, x + w, y + h, current_color);
	}

	return 0;
}

int tsab_graphics_ellipse(lua_State *L) {
	if (tsab_shaders_get_active() > -1) {
		GPU_SetUniformf(GPU_GetUniformLocation(tsab_shaders_get_active_shader(), "textured"), 0);
	}

	double x = luaL_checknumber(L, 1);
	double y = luaL_checknumber(L, 2);
	double w = luaL_checknumber(L, 3);
	double h = luaL_checknumber(L, 4);
	double d = check_number(L, 5, 0);
	bool filled = check_bool(L, 6, true);

	if (filled) {
		GPU_EllipseFilled(current_target == nullptr ? screen : current_target->target, x, y, w, h, d, current_color);
	} else {
		GPU_Ellipse(current_target == nullptr ? screen : current_target->target, x, y, w, h, d, current_color);
	}

	return 0;
}

int tsab_graphics_triangle(lua_State *L) {
	if (tsab_shaders_get_active() > -1) {
		GPU_SetUniformf(GPU_GetUniformLocation(tsab_shaders_get_active_shader(), "textured"), 0);
	}

	double x1 = luaL_checknumber(L, 1);
	double y1 = luaL_checknumber(L, 2);
	double x2 = luaL_checknumber(L, 3);
	double y2 = luaL_checknumber(L, 4);
	double x3 = luaL_checknumber(L, 5);
	double y3 = luaL_checknumber(L, 6);

	bool filled = check_bool(L, 7, true);

	if (filled) {
		GPU_TriFilled(current_target == nullptr ? screen : current_target->target, x1, y1, x2, y2, x3, y3, current_color);
	} else {
		GPU_Tri(current_target == nullptr ? screen : current_target->target, x1, y1, x2, y2, x3, y3, current_color);
	}

	return 0;
}

int tsab_graphics_point(lua_State *L) {
	if (tsab_shaders_get_active() > -1) {
		GPU_SetUniformf(GPU_GetUniformLocation(tsab_shaders_get_active_shader(), "textured"), 0);
	}

	double x = luaL_checknumber(L, 1);
	double y = luaL_checknumber(L, 2);

	GPU_Pixel(current_target == nullptr ? screen : current_target->target, x + 0.5, y + 0.5, current_color);

	return 0;
}

int tsab_graphics_line(lua_State *L) {
	if (tsab_shaders_get_active() > -1) {
		GPU_SetUniformf(GPU_GetUniformLocation(tsab_shaders_get_active_shader(), "textured"), 0);
	}

	double x1 = luaL_checknumber(L, 1);
	double y1 = luaL_checknumber(L, 2);
	double x2 = luaL_checknumber(L, 3);
	double y2 = luaL_checknumber(L, 4);

	GPU_Line(current_target == nullptr ? screen : current_target->target, x1 + 0.5, y1 + 0.5, x2 + 0.5, y2 + 0.5, current_color);

	return 0;
}

int tsab_graphics_set_clear_color(lua_State *L) {
	double r = check_number(L, 1, 1);
	double g = check_number(L, 2, 1);
	double b = check_number(L, 3, 1);

	bg_color[0] = clamp(r * 255, 0, 255);
	bg_color[1] = clamp(g * 255, 0, 255);
	bg_color[2] = clamp(b * 255, 0, 255);

	return 0;
}

int tsab_graphics_set_color(lua_State *L) {
	double r = check_number(L, 1, 1);
	double g = check_number(L, 2, 1);
	double b = check_number(L, 3, 1);
	double a = check_number(L, 4, 1);

	current_color.r = clamp(r * 255, 0, 255);
	current_color.g = clamp(g * 255, 0, 255);
	current_color.b = clamp(b * 255, 0, 255);
	current_color.a = clamp(a * 255, 0, 255);

	return 0;
}

int tsab_graphics_get_color(lua_State *L) {
	lua_pushnumber(L, ((double) current_color.r) / 255);
	lua_pushnumber(L, ((double) current_color.g) / 255);
	lua_pushnumber(L, ((double) current_color.b) / 255);
	lua_pushnumber(L, ((double) current_color.a) / 255);

	return 4;
}

static bool pushed;

int tsab_graphics_camera(lua_State *L) {
	double x = check_number(L, 1, 0);
	double y = check_number(L, 2, 0);
	double s = check_number(L, 3, 1);

	GPU_MatrixMode(GPU_MODELVIEW);

	if (pushed) {
		GPU_PopMatrix();
	}

	if (lua_isnil(L, 1)) {
		pushed = false;
		return 0;
	}

	if (current_target == nullptr) {
		y *= -1;
	}

	pushed = true;
	GPU_PushMatrix();
	GPU_Scale(s, s, 1.0f);
	GPU_Translate(x / (current_target == nullptr ? screen->w / 2 : current_target->w / 2), y / (current_target == nullptr ? screen->h / 2 : current_target->h / 2), 0.0f);
	return 0;
}

int tsab_graphics_new_font(lua_State *L) {
	const char *name = luaL_checkstring(L, 1);
	int size = check_number(L, 2, 14);

	TTF_Font *font = TTF_OpenFont(name, size);

	if (font == nullptr) {
		std::cerr << "Failed to load font " << name << ": " << TTF_GetError() << std::endl;
		return -1;
	}

	if (active_font == nullptr) {
		active_font = font;
	}

	fonts.push_back(font);
	lua_pushnumber(L, fonts.size() - 1);

	return 1;
}

int tsab_graphics_set_font(lua_State *L) {
	int id = luaL_checknumber(L, 1);

	if (id > - 1 && id < fonts.size()) {
		active_font = fonts[id];
	}

	return 0;
}

int tsab_graphics_print(lua_State *L) {
	if (tsab_shaders_get_active() > -1) {
		GPU_SetUniformf(GPU_GetUniformLocation(tsab_shaders_get_active_shader(), "textured"), 1);
	}

	const char *text = luaL_checkstring(L, 1);
	double x = check_number(L, 2, 0);
	double y = check_number(L, 3, 0);
	double r = check_number(L, 4, 0);
	double sx = check_number(L, 5, 1);
	double sy = check_number(L, 6, 1);

	SDL_Surface *surface = TTF_RenderUTF8_Blended(active_font, text, current_color);
	GPU_Image *image = GPU_CopyImageFromSurface(surface);
	GPU_BlitTransformX(image, nullptr, current_target == nullptr ? screen : current_target->target, x + image->w/2, y + image->h/2, image->w/2, image->h/2, r, sx, sy);
	SDL_FreeSurface(surface);

	return 0;
}

int tsab_graphics_get_size(lua_State *L) {
	int id = check_number(L, 1, -2);

	if (id == -2) {
		int w;
		int h;

		SDL_GetWindowSize(window, &w, &h);
		lua_pushnumber(L, w);
		lua_pushnumber(L, h);
	} else if (id == -1) {
		lua_pushnumber(L, screen->w);
		lua_pushnumber(L, screen->h);
	} else if (id > -1 && id < canvas_list.size()) {
		GPU_Image *image = canvas_list[id];

		lua_pushnumber(L, image->w);
		lua_pushnumber(L, image->h);
	} else {
		return 0;
	}

	return 2;
}

int tsab_graphics_set_size(lua_State *L) {
	int w = luaL_checknumber(L, 1);
	int h = luaL_checknumber(L, 2);

	GPU_SetWindowResolution(w, h);
	return 0;
}

int tsab_graphics_set_title(lua_State *L) {
	const char *title = luaL_checkstring(L, 1);

	SDL_SetWindowTitle(window, title);
	return 0;
}