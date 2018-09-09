CANVAS_WIDTH = 256
CANVAS_HEIGHT = math.floor(CANVAS_WIDTH / 3 * 2)
PIXEL_PERFECT = false

local default_scale = 2

config = {
	window = {
		width = CANVAS_WIDTH * default_scale,
		height = CANVAS_HEIGHT * default_scale,
		min_width = CANVAS_WIDTH,
		min_height = CANVAS_HEIGHT,
		title = "not so bundle entry - by @egordorichev"
	},

	extensions = {
		physics = true,
		discord = true,
		steam = true
	}
}