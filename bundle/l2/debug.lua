local debug = {}

debug.active = false
print = log.info

function debug.draw_log()
	tsab.ui.begin("Log")

	for _, v in pairs(log.list) do
		tsab.ui.label(v)
	end

	tsab.ui.finish()
end

local filter, changed = "", false

function debug.draw_entities()
	tsab.ui.begin("Entities")
	filter, changed = tsab.ui.text_input("Filter", filter)

	if game.state and game.state.area and #game.state.area.entities > 0 then
		for _, v in pairs(game.state.area.entities) do
			if v.type_name:find(filter, 0, true) then
				if tsab.ui.collapsing_header(v.type_name or "entity") then
					if v.hp then
						tsab.ui.label("HP", v.hp .. "/" .. v.max_hp)
					end

					v:set(tsab.ui.input_float2("pos", v.x, v.y))

					if v.vel then
						v.vel:set(tsab.ui.input_float2("vel", v.vel.x, v.vel.y))
					end

					if tsab.ui.button("Kill") then
						v.done = true
					end

					if v.modify_hp then
						tsab.ui.same_line()

						if tsab.ui.button("Hurt") then
							v:modify_hp(-1)
						end
					end

					if v.hp then
						tsab.ui.same_line()

						if tsab.ui.button("Heal") then
							v:modify_hp(v.max_hp)
						end
					end

					if tsab.ui.tree_node("halp") then
						tsab.ui.pop_node()
					end
				end
			end
		end
	else
		tsab.ui.label("No entities")
	end

	tsab.ui.finish()
end

function debug.draw_main()
	local cnt = lume.round(collectgarbage("count") / 1000, 0.01)

	tsab.ui.begin("Debug")
	tsab.ui.label("FPS", tostring(math.floor(tsab.get_fps())))
	tsab.ui.label("Memory", cnt .. " mb")
	tsab.ui.label("State", game.state and game.state.name or "none")

	if game.state and game.state.area then
		tsab.ui.label("Entities", tostring(#game.state.area.entities))
	end

	tsab.ui.finish()
end

function debug.draw()
	if tsab.input.was_pressed("f1") then
		debug.active = not debug.active
	end

	if not debug.active then return end

	tsab.ui.frame()
	debug.draw_log()
	debug.draw_entities()
	debug.draw_main()
	tsab.ui.render()
end

return debug