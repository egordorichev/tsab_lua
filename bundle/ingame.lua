local area = require "l2.area"
local state = require "l2.state"
local ingame = state:extend()
ingame.name = "ingame"

local player = require "player"
local level = require "l2.level"

function ingame:init()
	tsab.physics.new_world(0, 40)
	self.area = area()

	local lvl = level("data/map/test", "data/image/tiles.png")

	self.area:add(lvl)

	local plr = player()

	self.area:add(plr)
	plr:set(96, 64)

	print("Loaded level")

	tsab.physics.new_body("static", {
		shape = "rect",
		x = 0,
		y = 110,
		w = CANVAS_WIDTH,
		h = 16
	})
end

function ingame:destroy()
	self.area:destroy()
	tsab.physics.destroy_world()
end

function ingame:update(dt)
	if tsab.input.was_pressed("f2") then
		game.reset()
	end

	tsab.physics.update(dt)
	self.area:update(dt)
end

function ingame:draw()
	self.area:draw()
	tsab.physics.draw()
end

return ingame