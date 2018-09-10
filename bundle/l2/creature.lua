local entity = require "l2.entity"
local creature = entity:extend()

function creature:new()
	creature.super.new(self)

	self.max_hp = 1
	self.hp = self.max_hp
	self.dead = false
end

function creature:die()
	if self.dead then return end

	self.dead = true
	self.hp = 0
	self.done = true
end

function creature:modify_hp(a)
	if self.dead then return end
	self.hp = lume.clamp(self.hp + a, 0, self.max_hp)

	if self.hp == 0 then
		self:die()
	end
end

return creature