local area = require "l2.area"
local state = require "l2.state"
local ingame = state:extend()

local player = require "player"

function ingame:init()
	tsab_physics_new_world()

	self.area = area()

	local plr = player()

	plr.x = 64
	plr.y = 64

	self.area:add(plr)
end

function ingame:destroy()
	tsab_physics_destroy_world()
	self.area:destroy()
end

function ingame:update(dt)
	tsab_physics_update()
	self.area:update(dt)
end

function ingame:draw()
	-- tsab_physics_draw()
	self.area:draw()
end

return ingame