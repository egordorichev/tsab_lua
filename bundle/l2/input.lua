local input = object:extend()

function input:new(keys)
	self.keys = {}
	if not keys then return end

	for k, v in pairs(keys) do
		self:bind_key(k, v)
	end
end

function input:bind_key(name, keys)
	local k = keys

	if type(k) ~= "table" then
		k = { keys }
	end

	local reg = self.keys[name]

	if not reg then
		self.keys[name] = {}
		reg = self.keys[name]
	end

	for _, n in pairs(k) do
		if type(n) == "string" then
			table.insert(reg, n)
		end
	end
end

return input