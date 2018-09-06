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
	tsab.graphics.circle(32, 32, 32)
	tsab.graphics.set_canvas(nil)
	tsab.graphics.draw(canvas, {0,0,32,32}, 0, 0, 0, 0, 0, 2, 2)
end