--
-- Init stuff
--

local canvas

function tsab.init()
	print("hi!")
	canvas = tsab.graphics.new_canvas(640 / 2, 480 / 2)
end

function tsab.destroy()
	print("bye!")
end

function tsab.error(message)
	print("tsab_error: " .. message)
	tsab.quit()
end

--
-- Main game loop
--

function tsab.update(dt)

end

function tsab.draw()
	tsab.graphics.set_canvas(canvas)
	tsab.graphics.rectangle(0, 0, 32, 32)
	tsab.graphics.set_canvas()

	tsab.graphics.color(1, 0.5, 0.5, 1)
	tsab.graphics.rectangle(32, 32, 64, 64)
	tsab.graphics.color(1, 1, 1, 0.5)
	tsab.graphics.draw(canvas, 0, 0, 0, 0, 0, 2, 2)
	tsab.graphics.color()
end