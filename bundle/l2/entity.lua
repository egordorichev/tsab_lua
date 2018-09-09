local rect = require "l2.rect"
local entity = rect:extend()

function entity:new()
	entity.super.new(self, 0, 0, 16, 16)

	self.depth = 0
	self.done = false
end

function entity:init()

end

function entity:destroy()

end

function entity:update(dt)

end

function entity:draw()

end

return entity