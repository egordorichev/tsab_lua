-- version
VERSION_MAJOR = 0.0
VERSION_MINOR = 0.0
VERSION_STRING = VERSION_MAJOR .. "." .. VERSION_MINOR

-- canvas
CANVAS_WIDTH = 160 * 2
CANVAS_HEIGHT = 90 * 2
PIXEL_PERFECT = true
local default_scale = 3

-- config
config = {
	window = {
		width = CANVAS_WIDTH * default_scale,
		height = CANVAS_HEIGHT * default_scale,
		min_width = CANVAS_WIDTH,
		min_height = CANVAS_HEIGHT,
		title = "not so metro " .. VERSION_STRING .. " by @egordorichev"
	},

	extensions = {
		physics = true,
		discord = true,
		steam = true
	}
}