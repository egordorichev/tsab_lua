local point = require "l2.point"
local creature = require "l2.creature"
local player = creature:extend()

function player:init()
	player.super.init(self)

	self.vel = point()
	self.speed = 40
	self.body = tsab.physics.new_body("dynamic", {
		shape = "rect",
		w = 16,
		h = 16
	})
end

function player:destroy()
	self.body:destroy()
end

function player:update(dt)
	player.super.update(self, dt)

	self:set(self.body:get_transform())
	self.vel:set(self.body:get_velocity())

	if input:is_down("left") then
		self.vel.x = self.vel.x - self.speed
	end

	if input:is_down("right") then
		self.vel.x = self.vel.x + self.speed
	end

	local vdt = dt * 60 * 0.7

	self.vel.x = self.vel.x * vdt
	self.body:set_velocity(self.vel.x, self.vel.y)
end

function player:draw()
	tsab.graphics.color(0.5, 0.5, 0.5, 1)
	tsab.graphics.rectangle(self.x, self.y, self.w, self.h)
end

return player