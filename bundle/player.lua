local point = require "l2.point"
local creature = require "l2.creature"
local player = creature:extend()

player.type_name = "player"

function player:init()
	player.super.init(self)

	self.vel = point()
	self.speed = 40 * 10
	self.jump_speed = 40 * 10
	self.w = 16
	self.h = 16

	self.body = tsab.physics.new_body("dynamic", {
		-- todo: polygon shapes
		shape = "rect",
		w = self.w,
		h = self.h
	})
end

function player:destroy()
	self.body:destroy()
end

function player:set(...)
	player.super.set(self, ...)

	if self.body then
		self.body:set_transform(self.x, self.y, 0)
	end
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

	if input:was_pressed("jump") then
		self.vel.y = self.vel.y - self.jump_speed
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