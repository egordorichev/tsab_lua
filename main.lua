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
	os.exit()
end

--
-- Main game loop
--

function tsab_update(dt)
	if tsab_input_was_pressed("q") then
		tsab_quit()
		return
	end

	--[[ if tsab_input_is_down("a") then
		print("a key is down")
	end

	if tsab_input_was_released("a") then
		print("a was released")
	end ]]
end

function tsab_draw()

end