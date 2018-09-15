-- version
VERSION_MAJOR = 0.0
VERSION_MINOR = 0.0
VERSION_STRING = VERSION_MAJOR .. "." .. VERSION_MINOR
DEBUG = true

-- canvas
CANVAS_WIDTH = 640
CANVAS_HEIGHT = 360
PIXEL_PERFECT = true
local default_scale = 2

-- config
config = {
	window = {
		vsync = false,

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