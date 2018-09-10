local debug = {}

debug.active = true
print = log.info

function debug.draw_log()
	tsab.ui.begin("Log")

	for _, v in pairs(log.list) do
		tsab.ui.label(v)
	end

	tsab.ui.finish()
end

function debug.draw_entities()
	tsab.ui.begin("Entities")

	tsab.ui.finish()
end

function debug.draw_main()
	local cnt = lume.round(collectgarbage("count") / 1000, 0.01)

	tsab.ui.begin("Debug")
	tsab.ui.label("FPS", tostring(math.floor(tsab.get_fps())))
	tsab.ui.label("Memory", cnt .. " mb")

	tsab.ui.finish()
end

function debug.draw()
	-- if not debug.active then return end

	tsab.ui.frame()
	debug.draw_log()
	debug.draw_entities()
	debug.draw_main()
	tsab.ui.render()
end

return debug