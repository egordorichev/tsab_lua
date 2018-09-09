local creature = require "l2.creature"
local player = creature:extend()

function player:draw()
	tsab.graphics.rectangle(self.x, self.y, self.w, self.h)
end

function player:update(dt)
	player.super.update(self, dt)

	if input.is_down("left") then
		self.x = self.x - 1
	end
end

return player