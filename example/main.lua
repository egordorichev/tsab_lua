--
-- Init stuff
--

local canvas
local sfx

function tsab.init()
	print("hi!")
	canvas = tsab.graphics.new_canvas(640 / 2, 480 / 2)

	-- tsab_audio_play_music(tsab_audio_new_music("music.mp3"))

	sfx = tsab_audio_new_sfx("sfx.mp3")
end

function tsab.destroy()
	print("bye!")
end

function tsab.error(message)
	print("tsab_error: " .. message)
end

--
-- Main game loop
--

local time = 0

function tsab.update(dt)
	time = time + dt

	if tsab.input.was_pressed("space") then
		tsab_audio_play_sfx(sfx)
	end
end

function tsab.draw()
	tsab.graphics.print("Hello, world!", 64, 64, time * 32, math.abs(math.sin(time)) / 2 + 0.5, math.abs(math.cos(time)) / 2 + 0.5)
end