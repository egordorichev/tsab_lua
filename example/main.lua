function tsab.draw()
	local time = tsab.get_time()
	tsab.graphics.print(tsab.get_fps())
	tsab.graphics.print("Hello, world!", 96, 96, time * 32, math.abs(math.sin(time)) / 2 + 0.5, math.abs(math.cos(time)) / 2 + 0.5)
end