local entity = require "l2.entity"
local level = entity:extend()

function level:new(path)
	path = path or "map.tmx"
end

function level:draw()

end

return level