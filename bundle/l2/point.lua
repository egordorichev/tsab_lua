local point = object:extend()

function point:new(x, y)
	self.x = x or 0
	self.y = y or x or 0
end

function point:__add(o)
	return point(self.x + o.x, self.y + o.y)
end

function point:__sub(o)
	return point(self.x - o.x, self.y - o.y)
end

function point:__div(o)
	return point(self.x / o.x, self.y / o.y)
end

function point:__mul(o)
	return point(self.x * o.x, self.y * o.y)
end

function point:get_dx_dy_to(x, y)
	if type(x) == "table" then
		if x:is(point) then
			return x.x - self.x, x.y - self.y
		elseif x:is(require "l2.rect")	then
			return x.cx() - self.x, x.cy() - self.y
		end

		return 0, 0
	end

	return x - self.x, y - self.y
end

function point:__len()
	return math.sqrt(self.x * self.x + self.y * self.y)
end

function point:distance_squared_to(x, y)
	local dx, dy = self:get_dx_dy_to(x, y)
	return dx * dx + dy * dy
end

function point:angle_to(x, y)
	local dx, dy = self:get_dx_dy_to(x, y)
	return math.atan2(dy, dx)
end

function point:distance_to(x, y)
	return math.sqrt(self:distance_squared_to(x, y))
end

function point:__tostring()
	return "point(" .. self.x .. ", " .. self.y .. ")"
end

return point