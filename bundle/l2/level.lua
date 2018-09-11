local entity = require "l2.entity"
local level = entity:extend()

function level:new(path, tile_image, tile_layer)
	local tile_layer = tile_layer

	if not tile_layer then
		tile_layer = "tiles"
	end

	local file = require(path)

	self.w = file.width * 16
	self.h = file.height * 16
	self.tw = file.width
	self.th = file.height
	self.sz = self.tw * self.th
	self.t = file.tilewidth
	self.texture = tsab.graphics.new_image(tile_image)
	self.ntr = self.texture.w / self.t

	self.data = {}
	local tiles

	for _, v in pairs(file.layers) do
		if v.name == tile_layer then
			tiles = v
		end
	end

	for i = 1, self.tw * self.th do
		self.data[i] = tiles.data[i]
	end

	self:add_physics()
end

function level:add_physics()

end

function level:draw()
	tsab.graphics.color(1, 1, 1, 1)

	for y = 0, self.th - 1 do
		for x = 0, self.tw - 1 do
			local i = x + y * self.tw
			local tile = self.data[i + 1] - 1

			if tile > -1 then
				tsab.graphics.draw(self.texture, { tile % self.ntr * self.t, tile // self.ntr * self.t, self.t, self.t }, x * self.t, y * self.t)
			end
		end
	end
end

return level