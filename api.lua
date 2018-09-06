tsab = {}
tsab.quit = tsab_quit

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
tsab.graphics.set_canvas = function(canvas) tsab_active_canvas = canvas or tsab_default_canvas end
tsab.graphics.new_canvas = function(w, h)
	local canvas = tsab_graphics_new_canvas(w, h)
	local ins = make_canvas(canvas, w, h)

	table.insert(tsab_all_canvases, ins)

	return ins
end

tsab.graphics.circle = function(x, y, r)
	tsab_graphics_circle(tsab_active_canvas.pointer, x, y, r)
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