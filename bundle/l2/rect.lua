local point = require "l2.point"
local rect = point:extend()

function rect:new(x, y, w, h)
	self:set(x, y, w, h)
end

function rect:set(x, y, w, h)
	rect.super.set(self, x, y)

	if w and h then
		self.w = w
		self.h = h
	end
end

function rect:contains(px, py)
	local x, y = px, py

	if type(px) == "table" then
		x = px.x
		y = px.y
	end

	return x >= self.x and x <= self.x + self.w
		and y >= self.y and y <= self.y + self.h
end

function rect:intersecrts(rx, ry, rw, rh)
	local x, y, w, h = rx, ry, rw, rh

	if type(rx) == "table" then
		x = rx.x
		y = rx.y
		w = rx.w
		h = rx.h
	end

	return self.x < x + w and self.x + self.w > x
		and self.y < y + h and self.y > y
end

function rect:cx()
	return self.x + self.w / 2
end

function rect:cy()
	return self.y + self.h / 2
end

function rect:get_dx_dy_to(x, y)
	if type(x) == "table" and x:is(rect) then
		return x:cx() - self:cx(), x:cy() - self:cy()
	end

	return rect.super.get_dx_dy_to(self, x, y)
end

function rect:__tostring()
	return "rect(" .. self.x .. ", " .. self.y .. ", " .. self.w .. ", " .. self.h .. ")"
end

return rect