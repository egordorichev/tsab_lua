--
-- Init stuff
--

function tsab_init()
	print("hi!")
end

function tsab_destroy()
	print("bye!")
end

function tsab_error(message)
	print("tsab_error: " .. message)
	tsab_quit()
end

--
-- Main game loop
--

function tsab_update(dt)
	if tsab_input_was_pressed("escape") then
		tsab_quit()
		return
	end
end

function tsab_draw()

end