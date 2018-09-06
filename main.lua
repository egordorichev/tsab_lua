--
-- Init stuff
--

local canvas

function tsab_init()
	print("hi!")
	canvas = tsab.graphics.new_canvas(640 / 2, 480 / 2)
end

function tsab_destroy()
	print("bye!")
end

function tsab_error(message)
	print("tsab_error: " .. message)
	tsab.quit()
end

--
-- Main game loop
--

function tsab_update(dt)
	if tsab.input.was_pressed("escape") then
		tsab.quit()
		return
	end
end

function tsab_draw()
	tsab.graphics.set_canvas(canvas)

	tsab.graphics.set_color(1, 0, 0.3, 1)
	tsab.graphics.rectangle(32, 32, 64, 32)
	tsab.graphics.set_color(0, 1, 0, 1)
	tsab.graphics.circle(64, 64, 16)

	tsab.graphics.set_canvas(nil)
	tsab.graphics.set_color()
	tsab.graphics.draw(canvas, 0, 0, 0, 0, 0, 2, 2)
end