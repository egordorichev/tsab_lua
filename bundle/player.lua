local creature = require "l2.creature"
local player = creature:extend()

function player:draw()
	tsab.graphics.rectangle(self.x, self.y, self.w, self.h)
end

return player