local area = require "l2.area"
local state = require "l2.state"
local ingame = state:extend()

local player = require "player"
local level = require "l2.level"

function ingame:init()
	tsab.physics.new_world()
	self.area = area()

	local lvl = level("data/map/test", "data/image/tiles.png")

	self.area:add(lvl)

	local plr = player()

	plr.x = 64
	plr.y = 64

	self.area:add(plr)
end

function ingame:destroy()
	self.area:destroy()
	tsab.physics.destroy_world()
end

function ingame:update(dt)
	tsab.physics.update()
	self.area:update(dt)
end

function ingame:draw()
	self.area:draw()
	tsab.physics.draw()
end

return ingame