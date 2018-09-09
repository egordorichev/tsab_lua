local game = {}

function game.set_state(state)
	game.apply_state(state)
end

function game.apply_state(state)
	if game.state then
		game.state:destroy()
	end

	game.state = state

	if game.state then
		game.state:init()
	end
end

function game.init(state)
	game.set_state(state)
end

function game.destroy()
	if game.state then
		game.state:destroy()
	end
end

function game.update(dt)
	if game.to_set ~= nil then
		game.apply_state(game.to_set)
		game.to_set = nil
	end

	if game.state then
		game.state:update(dt)
	end
end

function game.draw()
	if game.state then
		game.state:draw()
	end
end

return game