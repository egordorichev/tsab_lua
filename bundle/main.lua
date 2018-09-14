-- libs are global, so require them before everything else
lume = require "lib.lume"
object = require "lib.classic"
flux = require "lib.flux"
tick = require "lib.tick"
lurker = require "lib.lurker"
log = require "lib.log"

local ENABLE_CANVAS = true

-- requires
game = require "l2.game" -- also global
local ingame = require "ingame"
local debug = require "l2.debug"

local canvas
local canvas_x
local canvas_y
local canvas_scale

function tsab.init()
	input = require "l2.input" ({
		left = { "left", "a" },
		right = { "right", "d" },
		up = { "up", "w" },
		down = { "down", "s" },
		jump = { "x", "space" }
	})

	canvas = tsab.graphics.new_canvas(CANVAS_WIDTH, CANVAS_HEIGHT)
	tsab.resize(tsab.graphics.get_size())
	tsab.graphics.background_color("#f6ca9c")

	game.init(ingame())
end

function tsab.destroy()
	game.destroy()
end

function tsab.update(dt)
	flux.update(dt)
	lurker.update(dt)
	tick.update(dt)

	game.update(dt)
end

function tsab.draw()
	if ENABLE_CANVAS then
		tsab.graphics.set_canvas(canvas)
		tsab.graphics.clear()
	end

	game.draw()

	if ENABLE_CANVAS then
		tsab.graphics.set_canvas()
		tsab.graphics.color()
		tsab.graphics.draw(canvas, canvas_x, canvas_y, 0, 0, 0, canvas_scale, canvas_scale)
	end

	debug.draw()
end

function tsab.resize(w, h)
	canvas_scale = math.min(w / CANVAS_WIDTH, h / CANVAS_HEIGHT)

	if PIXEL_PERFECT then
		canvas_scale = math.floor(canvas_scale)
	end

	canvas_x = (w - canvas_scale * CANVAS_WIDTH) / 2
	canvas_y = (h - canvas_scale * CANVAS_HEIGHT) / 2
end