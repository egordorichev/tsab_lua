tsab = {}
tsab.quit = tsab_quit
tsab.update = function() end
tsab.draw = function() end
tsab.destroy = function() end
tsab.error = function() end

tsab_init = function() tsab.init() end
tsab_update = function(dt) tsab.update(dt) end
tsab_destroy = function() tsab.destroy() end
tsab_error = function() tsab.error() end

--
-- input
--

tsab.input = {}
tsab.input.is_down = tsab_input_is_down
tsab.input.was_pressed = tsab_input_was_pressed
tsab.input.was_released = tsab_input_was_released
tsab.input.get_axis = tsab_input_get_axis

--
-- graphics
--

tsab.graphics = {}

local function make_canvas(canvas, w, h)
	return {
		type = "canvas",
		pointer = canvas,
		w = w,
		h = h
	}
end

local canvas, w, h = tsab_graphics_get_default_canvas()
local tsab_default_canvas = make_canvas(canvas, w, h)
tsab_default_canvas.screen = true

local tsab_active_canvas = tsab_default_canvas
local tsab_all_canvases = {
	tsab_default_canvas
}

tsab.graphics.get_canvas = function() return tsab_active_canvas end

tsab.graphics.set_canvas = function(canvas)
	tsab_active_canvas = canvas or tsab_default_canvas

	if canvas then
		tsab_graphics_set_canvas(canvas.pointer)
	else
		tsab_graphics_set_canvas(nil)
	end
end

tsab.graphics.new_canvas = function(w, h)
	local canvas = tsab_graphics_new_canvas(w, h)
	local ins = make_canvas(canvas, w, h)

	table.insert(tsab_all_canvases, ins)

	return ins
end

local active_shader
tsab.graphics.color = function(r, g, b, a)
	tsab_graphics_set_color(r, g, b, a)

	if active_shader then
		tsab_shaders_send_vec4(active_shader, "color", r or 1, g or 1, b or 1, a or 1)
	end
end

tsab.graphics.get_color = tsab_graphics_get_color
tsab.graphics.circle = tsab_graphics_circle
tsab.graphics.rectangle = tsab_graphics_rectangle
tsab.graphics.point = tsab_graphics_point
tsab.graphics.line = tsab_graphics_line
tsab.graphics.ellipse = tsab_graphics_ellipse
tsab.graphics.triangle = tsab_graphics_triangle
tsab.graphics.camera = tsab_graphics_camera
tsab.graphics.new_font = tsab_graphics_new_font
tsab.graphics.set_font = tsab_graphics_set_font

tsab.graphics.print = function(str, x, y, r, sx, sy)
	if tsab_active_canvas.screen then
		tsab_graphics_print(tsab_active_canvas.pointer, str, x, y, r, sx, sy)
	else
		tsab_graphics_print_to_texture(tsab_active_canvas.pointer, str, x, y, r, sx, sy)
	end
end

tsab.graphics.draw = function(what, x, y, a, ox, oy, sx, sy, e)
	if tsab_active_canvas.screen then
		if type(x) == "table" then
			local src_x = x.x or x[1]
			local src_y = x.y or x[2]
			local src_w = x.w or x[3]
			local src_h = x.h or x[4]

			tsab_graphics_draw(tsab_active_canvas.pointer, what.pointer, y, a, ox, oy, sx, sy, e, src_x, src_y, src_w, src_h)
		else
			tsab_graphics_draw(tsab_active_canvas.pointer, what.pointer, x, y, a, ox, oy, sx, sy, 0, 0, what.w, what.h)
		end
	else
		if type(x) == "table" then
			local src_x = x.x or x[1]
			local src_y = x.y or x[2]
			local src_w = x.w or x[3]
			local src_h = x.h or x[4]

			tsab_graphics_draw_to_texture(tsab_active_canvas.pointer, what.pointer, y, a, ox, oy, sx, sy, e, src_x, src_y, src_w, src_h)
		else
			tsab_graphics_draw_to_texture(tsab_active_canvas.pointer, what.pointer, x, y, a, ox, oy, sx, sy)
		end
	end
end

--
-- shaders
--

tsab.shaders = {}
tsab.shaders.new = tsab_shaders_new

tsab.shaders.set = function(v)
	active_shader = v
	tsab_shaders_set(v)
end

local default_shader = 	tsab.shaders.new("default.frag")
tsab.shaders.set(default_shader)
tsab.graphics.color()
tsab.shaders.set()
tsab.shaders.send_vec4 = tsab_shaders_send_vec4
tsab.shaders.send_float = tsab_shaders_send_float

tsab.shaders.get_default = function() return default_shader end

function tsab_init()
	tsab.graphics.new_font("font.ttf", 18)
	tsab.init()
end

function tsab_draw()
	tsab.shaders.set(default_shader)
	tsab.draw()
	tsab.shaders.set()
end

--
-- audio
--

tsab.audio = {}

tsab.audio.new_sfx = function(...)
	local sfx = tsab_audio_new_sfx(...)

	return {
		type = "sfx",
		pointer = sfx
	}
end

tsab.audio.new_music = function(...)
	local music = tsab_audio_new_music(...)

	return {
		type = "music",
		pointer = music
	}
end

tsab.audio.play = function(what, loop)
	if what.type == "music" then
		tsab_audio_play_music(what.pointer, loop)
	else
		tsab_audio_play_sfx(what.pointer)
	end
end

tsab.audio.set_volume = function(what, val)
	if type(what) == "table" then
		if what.type == "music" then
			tsab_audio_set_music_volume(val)
		else
			tsab_audio_set_sfx_volume(what.pointer, val)
		end
	else
		tsab_audio_set_general_volume(what, val)
	end
end