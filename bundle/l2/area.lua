local area = object:extend()

function area:new()
	self.entities = {}
end

function area:add(e)
	if not e or e.done then return end

	table.insert(self.entities, e)
	e:init()
end

function area:clear()
	for _, e in pairs(self.entities) do
		e:destroy()
	end

	self.entities = {}
end

function area:destroy()
	self:clear()
end

function area:update(dt)
	for i = #self.entities, 1, -1 do
		local e = self.entities[i]
		e:update(dt)

		if e.done then
			e:destroy()
			table.remove(self.entities, i)
		end
	end
end

function area:draw()
	for _, e in pairs(self.entities) do
		e:draw()
	end
end

return area