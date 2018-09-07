local canvas = tsab.graphics.new_canvas(128, 128)
tsab.graphics.background_color(0.1, 0.1, 0.1)
local image = tsab.graphics.new_image("sprites.png")

function tsab.draw()
	local time = tsab.get_time()

	tsab.graphics.set_canvas(canvas)
	tsab.graphics.clear()
	tsab.graphics.draw(image, 64, 64, 45, 64, 64)
	tsab.graphics.print(tsab.get_fps())
	tsab.graphics.set_canvas()
	tsab.graphics.draw(canvas, 0, 0, 0, 0, 0, 5, 5)
	tsab.graphics.print("Hello, world!", 96, 96, time * 32, math.abs(math.sin(time)) / 2 + 0.5, math.abs(math.cos(time)) / 2 + 0.5)
end