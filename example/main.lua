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
	tsab.graphics.print("hello, world!", 10, 10)
end