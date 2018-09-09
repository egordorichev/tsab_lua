local area = require "l2.area"
local state = require "l2.state"
local ingame = state:extend()

local player = require "player"

function ingame:init()
	self.area = area()
	self.area:add(player())
end

function ingame:destroy()
	self.area:destroy()
end

function ingame:update(dt)
	self.area:update(dt)
end

function ingame:draw()
	self.area:draw()
end

return ingame