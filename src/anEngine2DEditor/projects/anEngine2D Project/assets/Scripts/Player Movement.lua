PlayerMovement = {}

function PlayerMovement.setup()
	local obj = {}
	return obj
end

function PlayerMovement.initialize(self)
end

function PlayerMovement.update(self, dt)
	if (isKey(87)) then
		self.owner:transform().position.y = self.owner:transform().position.y - 1
	end

	if (isKey(83)) then
		self.owner:transform().position.y = self.owner:transform().position.y + 1
	end

	if (isKey(65)) then
		self.owner:transform().position.x = self.owner:transform().position.x - 1
	end

	if (isKey(68)) then
		self.owner:transform().position.x = self.owner:transform().position.x + 1
	end

	if (isKeyDown(32)) then
		closeApplication()
	end
end