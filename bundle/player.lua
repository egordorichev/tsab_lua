local point = require "l2.point"
local creature = require "l2.creature"
local player = creature:extend()

function player:init()
	player.super.init(self)

	self.vel = point()
	self.speed = 40
end

function player:update(dt)
	player.super.update(self, dt)

	if input:is_down("left") then
		self.vel.x = self.vel.x - self.speed
	end

	if input:is_down("right") then
		self.vel.x = self.vel.x + self.speed
	end

	self.x = self.x + self.vel.x * dt
	self.y = self.y + self.vel.y * dt

	local vdt = dt * 60 * 0.7

	self.vel.x = self.vel.x * vdt
	self.vel.y = self.vel.y * vdt
end

function player:draw()
	tsab.graphics.color(0.5, 1, 0.5, 1)
	tsab.graphics.rectangle(self.x, self.y, self.w, self.h)
end

return player