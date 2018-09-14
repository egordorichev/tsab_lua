tsab = {}
tsab.get_time = tsab_get_time
tsab.get_fps = tsab_get_fps
tsab.quit = tsab_quit

tsab.update = function() end
tsab.draw = function() end
tsab.destroy = function() end
tsab.resize = function(w, h) end

tsab.error = function(error)
	print("tsab_error: " .. error)
	tsab.quit()
end

tsab_init = function() tsab.init() end
tsab_update = function(dt) tsab.update(dt) end
tsab_destroy = function() tsab.destroy() end
tsab_error = function(error) tsab.error(error) end
tsab_resize = function(...) tsab.resize(...) end

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

	if type(canvas) == "table" then
		tsab_graphics_set_canvas(canvas.pointer)
	else
		tsab_graphics_set_canvas(-1)
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

tsab.graphics.background_color = tsab_graphics_set_clear_color

tsab.graphics.get_color = tsab_graphics_get_color
tsab.graphics.circle = tsab_graphics_circle
tsab.graphics.rectangle = tsab_graphics_rectangle
tsab.graphics.po= tsab_graphics_point
tsab.graphics.line = tsab_graphics_line
tsab.graphics.ellipse = tsab_graphics_ellipse
tsab.graphics.triangle = tsab_graphics_triangle
tsab.graphics.camera = tsab_graphics_camera
tsab.graphics.new_font = tsab_graphics_new_font
tsab.graphics.set_font = tsab_graphics_set_font
tsab.graphics.get_size = tsab_graphics_get_size
tsab.graphics.set_size = tsab_graphics_set_size
tsab.graphics.set_title = tsab_graphics_set_title
tsab.graphics.print = tsab_graphics_print
tsab.graphics.clear = tsab_graphics_clear

tsab.graphics.new_image = function(...)
	local img = tsab_graphics_new_image(...)
	local w, h = tsab_graphics_get_size(img)

	return {
		type = "image",
		pointer = img,
		w = w,
		h = h
	}
end

tsab.graphics.draw = function(what, x, y, a, ox, oy, sx, sy, e)
	if not what then return end

	if type(x) == "table" then
		local src_x = x.x or x[1]
		local src_y = x.y or x[2]
		local src_w = x.w or x[3]
		local src_h = x.h or x[4]

		tsab_graphics_draw(what.pointer, y, a, ox, oy, sx, sy, e, src_x, src_y, src_w, src_h)
	else
		tsab_graphics_draw(what.pointer, x, y, a, ox, oy, sx, sy, 0, 0, what.w, what.h)
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

local default_shader = 	tsab.shaders.new([[
varying vec2 texCoord;

uniform sampler2D tex;
uniform vec4 color;
uniform float textured;

void main() {
    if (textured > 0.5) {
        gl_FragColor = texture2D(tex, texCoord) * color;
    } else {
        gl_FragColor = color;
    }
}
]], true) -- true means that it wont read this as a file name, yet just compile it

tsab.shaders.set(default_shader)
tsab.graphics.color()
tsab.shaders.set()
tsab.shaders.send_vec4 = tsab_shaders_send_vec4
tsab.shaders.send_float = tsab_shaders_send_float

tsab.shaders.get_default = function() return default_shader end

function tsab_init()
	tsab.graphics.new_font("data/font/font.ttf", 16)

	if tsab.init then
		tsab.init()
	end
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

--
-- filesystem
--

tsab.fs = {}
tsab.fs.get_directory_files = tsab_fs_get_directory_files
tsab.fs.is_directory = tsab_fs_is_directory
tsab.fs.get_last_modified = tsab_fs_get_last_modified

--
-- physics
--

tsab.physics = {}
tsab.physics.new_world = tsab_physics_new_world
tsab.physics.destroy_world = tsab_physics_destroy_world
tsab.physics.update = tsab_physics_update
tsab.physics.draw = tsab_physics_draw

local object = require "lib.classic"
local body = object:extend()

function body:new(pointer)
	self.pointer = pointer
end

function body:destroy()
	tsab_physics_destroy_body(self.pointer)
	self.pointer = -1
end

function body:set_transform(...)
	tsab_physics_set_body_transform(self.pointer, ...)
end

function body:get_transform()
	return tsab_physics_get_body_transform(self.pointer)
end

function body:set_velocity(...)
	tsab_physics_set_body_velocity(self.pointer, ...)
end

function body:get_velocity()
	return tsab_physics_get_body_velocity(self.pointer)
end

function body:add_fixture(...)
	tsab_physics_add_fixture(self.pointer, ...)
end

function body:apply_force(...)
	tsab_physics_apply_force(self.pointer, ...)
end

tsab.physics.new_body = function(...)
	return body(tsab_physics_new_body(...))
end

tsab.physics.destroy_body = tsab_physics_destroy_body
tsab.physics.set_body_transform = tsab_physics_set_body_transform
tsab.physics.get_body_transform = tsab_physics_get_body_transform
tsab.physics.set_body_velocity = tsab_physics_set_body_velocity
tsab.physics.get_body_velocity =  tsab_physics_get_body_velocity

--
-- ui
--

tsab.ui = {}
tsab.ui.frame = tsab_ui_frame
tsab.ui.button = tsab_ui_button
tsab.ui.text_input = tsab_ui_text_input
tsab.ui.render = tsab_ui_render
tsab.ui.begin = tsab_ui_begin
tsab.ui.finish = tsab_ui_end
tsab.ui.label = tsab_ui_label
tsab.ui.tree_node = tsab_ui_tree_node
tsab.ui.pop_node = tsab_ui_pop_node
tsab.ui.collapsing_header = tsab_ui_collapsing_header
tsab.ui.same_line = tsab_ui_same_line
tsab.ui.slider_float2 = tsab_ui_slider_float2
tsab.ui.input_float2 = tsab_ui_input_float2
tsab.ui.slider_float = tsab_ui_slider_float
tsab.ui.input_float = tsab_ui_input_float